/*
 * CodeGenerator.cpp
 *
 *  Created on: May 22, 2019
 *      Author: derommo
 */

#include "CodeGenerator.h"

#include <iostream>
#include <string.h>
#include <stdint.h>
#include <cstdarg>

#include "GlobalDefines.h"
#include "Ring.h"
#include "Interface.h"
#include "Module.h"
#include "embeddedFiles.h"

#include "ControlTransfer.h"

#define DEBUG(...) printf(__VA_ARGS__)

#define fprintProtect(ret) \
	{if(0 > ret) \
	{ \
		Error("fprintf failed: %s\n", strerror(errno)); \
		return false; \
	} \
	}

#define SNPRINTF(pt, size, ...) \
	{ \
		int ret = snprintf(pt, size, __VA_ARGS__); \
		if(0 > ret) \
		{ \
			Error("snprintf failed: %s!\n", strerror(errno)); \
			return false; \
		} \
		else if((int) size <= ret) \
		{ \
			Error("snprintf failed: Buffer too small: Size %lu, need %u\n", size, ret + 1); \
			return false; \
		} \
	}

#define retFalseOnFalse(boolean, ...) \
		{ \
			if(!boolean) \
			{ \
				Error(__VA_ARGS__); \
				return false; \
			} \
		}

#define getVarRetFalseOnError(pt, nodeId) \
	Variable* pt = GetVariable(nodeId); \
	if(nullptr == pt) \
	{ \
		Error("Variable for Node%u does not exist!\n", nodeId); \
		return false; \
	} \


#define HEADER_NAME "dac.h"

static const char fileName[] = "dac";


static const char includeFilesBrackets[][42] = {
		"stdint.h",
		"stddef.h",
		"linux/types.h",
		"pthread.h"
};

static const char includeFilesQuotes[][42] = {
		HEADER_NAME,
		"Helpers.h",
		"error_functions.h",
};

static const char fileHeader[] = \
"/*\n\
* Distributed Algebraic Computations\n\
* Copyright (C) 2019  Patrik Omland\n\
* This program is free software: you can redistribute it and/or modify\n\
* it under the terms of the GNU General Public License as published by\n\
* the Free Software Foundation, either version 3 of the License, or\n\
* (at your option) any later version.\n\
*\n\
* This program is distributed in the hope that it will be useful,\n\
* but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
* GNU General Public License for more details.\n\
*\n\
* You should have received a copy of the GNU General Public License\n\
* along with this program. If not, see <http://www.gnu.org/licenses/>.\n\
*\n\
* This program was generated by DAC \"Distributed Algebraic Computations\"\n\
* The source code for the generator is available on GitHub\n\
* https://github.com/siquus/dac\n\
*/\n\
\n";

static const char nodesId[] = "nodes";
static const uint32_t NODE_T_MAX_EDGE_NUMBER = 42;

FileWriter::~FileWriter()
{
	if(nullptr != outfile_)
	{
		if(fclose(outfile_))
		{
			Error("fclose failed: %s\n", strerror(errno));
		}
	}
}

bool FileWriter::Init(const std::string * path)
{
	path_ = *path;
	outfile_ = fopen(path_.c_str(), "w");
	if(nullptr == outfile_)
	{
		Error("Open File %s failed: %s\n", path_.c_str(), strerror(errno));
		return false;
	}

	// Print standard header
	fprintProtect(fprintf(outfile_, "%s\n", fileHeader));

	return true;
}

void FileWriter::Indent(uint8_t tabNumber)
{
	indentationLevel_ += tabNumber;
}

void FileWriter::Outdent(uint8_t tabNumber)
{
	if(indentationLevel_ < tabNumber)
	{
		Error("Indentation %u does not allow outdent of %u",
				indentationLevel_, tabNumber);

		return;
	}

	indentationLevel_ -= tabNumber;
}

int FileWriter::PrintfLine(const char * format, ...)
{
	va_list argList;
	va_start(argList, format);

	std::string formatStr;
	for(uint8_t tab = 0; tab < indentationLevel_; tab++)
	{
		formatStr += "\t";
	}

	formatStr += format;
	formatStr += "\n";

	return vfprintf(outfile_, formatStr.c_str(), argList);
}

const std::string * FileWriter::Path() const
{
	return &path_;
}

CodeGenerator::CodeGenerator(const std::string* path) {
	path_ = *path;
}

CodeGenerator::~CodeGenerator() {

}

bool CodeGenerator::Generate(const Graph* graph, const Parallizer* parallizer)
{
	graph_ = graph;

	// From here on we assume that the graph will no longer change
	// this means we may use pointers to nodes now!
	auto nodes = graph_->GetNodes();
	for(const Node &node: *nodes)
	{
		nodeMap_.insert(std::make_pair(node.id, &node));
	}

	for(const auto &nodePair: nodeMap_)
	{
		DEBUG("nodeMap Node%2u: %s,\t\t ObjType %2u,  Children ",
				nodePair.first,
				Node::getName(nodePair.second->type),
				(unsigned int) nodePair.second->objectType);

		for(Node::Id_t nodeId: nodePair.second->children)
		{
			DEBUG("%u, ", nodeId);
		}
		DEBUG("Parents ");
		for(Node::Id_t nodeId: nodePair.second->parents)
		{
			DEBUG("%u, ", nodeId);
		}
		DEBUG("\n");
	}

	retFalseOnFalse(FetchVariables(), "Could not fetch variables\n");

	std::string pathAndFileName = path_ + fileName;
	std::string dacCPath = pathAndFileName + ".c";

	bool success = fileDacC_.Init(&dacCPath);
	if(!success)
	{
		Error("Could not initialize %s!", dacCPath.c_str());
		return false;
	}

	std::string dacHPath = pathAndFileName + ".h";
	success = fileDacH_.Init(&dacHPath);
	if(!success)
	{
		Error("Could not initialize %s!", dacHPath.c_str());
		return false;
	}

	std::string nodesPath = path_;
	nodesPath += "Nodes.c";
	success = fileNodes_.Init(&nodesPath);
	if(!success)
	{
		Error("Could not initialize %s!", nodesPath.c_str());
		return false;
	}

	std::string InstructionsPath = path_;
	InstructionsPath += "Instructions.c";
	success = fileInstructions_.Init(&InstructionsPath);
	if(!success)
	{
		Error("Could not initialize %s!", InstructionsPath.c_str());
		return false;
	}

	InstructionsPath = path_;
	InstructionsPath += "Instructions.h";
	success = fileInstructionsH_.Init(&InstructionsPath);
	if(!success)
	{
		Error("Could not initialize %s!", InstructionsPath.c_str());
		return false;
	}

	std::string CommonPath = path_;
	CommonPath += "Common.h";
	success = fileCommonH_.Init(&CommonPath);
	if(!success)
	{
		Error("Could not initialize %s!", CommonPath.c_str());
		return false;
	}

	// Copy files
	GenerateEmbeddedFiles(&path_);

	// Generate Includes
	retFalseOnFalse(GenerateIncludes(), "Could not generate Includes\n");
	fprintProtect(fileDacC_.PrintfLine(""));

	// Generate #defines
	auto cpuInfo = parallizer->GetCpuInfo();
	if(1 != cpuInfo->size())
	{
		Error("Only one CPU supported currently!\n");
		return false;
	}

	const Parallizer::cpu_t * cpu = &cpuInfo->begin()->second;
	fprintProtect(fileCommonH_.PrintfLine("#define THREADS_NROF %uu", cpu->coresNrOf));

	fprintProtect(fileInstructions_.PrintfLine("#include <stdint.h>\n"));
	fprintProtect(fileInstructions_.PrintfLine("#include \"dac.h\""));
	fprintProtect(fileInstructions_.PrintfLine("#include \"Nodes.h\"\n"));
	fprintProtect(fileInstructions_.PrintfLine("#include \"Helpers.h\"\n"));
	fprintProtect(fileInstructions_.PrintfLine("#include \"Instructions.h\"\n"));

	// Generate Functions
	fprintProtect(fileDacH_.PrintfLine("#include <stddef.h>\n"));
	retFalseOnFalse(GenerateOutputFunctions(), "Could not generate Output Functions\n!");
	fprintProtect(fileInstructions_.PrintfLine(""));

	// Generate Variables
	retFalseOnFalse(GenerateConstantDeclarations(), "Could not generate Constants\n");
	fprintProtect(fileInstructions_.PrintfLine(""));
	retFalseOnFalse(GenerateStaticVariableDeclarations(), "Could not generate Statics\n!");
	fprintProtect(fileInstructions_.PrintfLine(""));

	auto runHeading = std::string("The main run routine");
	retFalseOnFalse(GenerateHeading(&runHeading), "Run Heading failed!\n");

	retFalseOnFalse(GenerateRunFunction(), "Could not generate Run Function!\n");

	fprintProtect(fileNodes_.PrintfLine("#include <stdint.h>\n"));
	fprintProtect(fileNodes_.PrintfLine("#include \"Instructions.h\"\n"));
	fprintProtect(fileNodes_.PrintfLine("#include \"Nodes.h\""));

	retFalseOnFalse(GenerateInstructions(), "Could not generate Instructions!\n");

	retFalseOnFalse(GenerateNodesArray(), "Could not generate Nodes Array");

	return true;
}

bool CodeGenerator::GenerateNodesArray()
{
	fprintProtect(fileNodes_.PrintfLine("node_t %s[] = {", nodesId));
	fileNodes_.Indent();

	// Write array
	for(const auto &nodePair: nodesInstructionMap_)
	{
		// Only include parents/children which require an instruction
		std::vector<uint32_t> parentsArrayPosition;
		for(const Node::Id_t &parent: nodePair.second->parents)
		{
			if(nodesInstructionMap_.end() != nodesInstructionMap_.find(parent))
			{
				const auto &arrayPos = nodeArrayPos_.find(parent);
				if(nodeArrayPos_.end() == arrayPos)
				{
					Error("Couldn't find array position");
					return false;
				}

				parentsArrayPosition.push_back(arrayPos->second);
			}
		}

		std::vector<uint32_t> childrenArrayPosition;
		if(nodePair.second->type != Node::Type::CONTROL_TRANSFER_WHILE) // Control Transfers don't have children
		{
			for(const Node::Id_t &child: nodePair.second->children)
			{
				if(nodesInstructionMap_.end() != nodesInstructionMap_.find(child))
				{
					const auto &arrayPos = nodeArrayPos_.find(child);
					if(nodeArrayPos_.end() == arrayPos)
					{
						Error("Couldn't find array position");
						return false;
					}

					childrenArrayPosition.push_back(arrayPos->second);
				}
			}
		}

		retFalseOnFalse(
				GenerateNodesElem(
						nodePair.first,
						&parentsArrayPosition,
						&childrenArrayPosition),
				"Could not generate Nodes Element!");
	}

	fileNodes_.Outdent();
	fprintProtect(fileNodes_.PrintfLine("};\n"));

	// Create node-list to initialize job pool
	std::set<Node::Id_t> firstNodes;
	GetFirstNodesToExecute(&firstNodes);

	std::string jobPoolInitStr;
	jobPoolInitStr += "#define JOB_POOL_INIT {";

	for(const auto &node: firstNodes)
	{
		const auto &arrayPos = nodeArrayPos_.find(node);
		if(nodeArrayPos_.end() == arrayPos)
		{
			Error("Couldn't find array position");
			return false;
		}

		jobPoolInitStr += "&nodes[";
		jobPoolInitStr += std::to_string(arrayPos->second);
		jobPoolInitStr += "]";
		jobPoolInitStr += ", ";
	}

	jobPoolInitStr.erase(jobPoolInitStr.size() - 2); // Remove last ", "
	jobPoolInitStr += "}";

	fprintProtect(fileCommonH_.PrintfLine(jobPoolInitStr.c_str()));
	fprintProtect(fileCommonH_.PrintfLine("#define JOB_POOL_INIT_NROF %u", firstNodes.size()));

	return true;
}

bool CodeGenerator::GetFirstNodesToExecute(std::set<Node::Id_t> * nodeSet)
{
	// Find all nodes which do not have parents and create a set of their children
	std::set<Node::Id_t> roots;

	auto nodes = graph_->GetNodes();
	for(const Node &node: *nodes)
	{
		if(0 == node.parents.size())
		{
			roots.insert(node.id);
			std::copy(
					node.children.begin(), node.children.end(),
					std::inserter(*nodeSet, nodeSet->end()));
		}
	}

	// delete all children which have non-root parents.
	// i.e. these children may be executed right away.
	std::vector<Node::Id_t> nonFirstGenerationChildren;
	for(Node::Id_t childId: *nodeSet)
	{
		const auto childNodeIt =  nodeMap_.find(childId);
		if(nodeMap_.end() == childNodeIt)
		{
			Error("Unknown Child, NodeId = %u!\n", childId);
			return false;
		}

		const Node* childNode = childNodeIt->second;

		for(Node::Id_t parentId: childNode->parents)
		{
			if(roots.end() == roots.find(parentId))
			{
				// Child has one non-first-generation parent.
				nonFirstGenerationChildren.push_back(childId); // erase later
				break;
			}
		}
	}

	for(Node::Id_t childId: nonFirstGenerationChildren)
	{
		nodeSet->erase(childId);
	}

	return true;
}

bool CodeGenerator::GenerateInstructions()
{
	Node::Id_t arrayPos = 0;
	const auto nodes = graph_->GetNodes();
	for(const Node &node: *nodes)
	{
		// Does this node require a function?
		switch(node.type)
		{
		default:
			Error("Unknown Node-Type %u\n", (uint8_t) node.type);
			return false;

		case Node::Type::VECTOR:
			continue; // does not have instruction

		case Node::Type::CONTROL_TRANSFER_WHILE: // no break intended
		case Node::Type::VECTOR_ADDITION: // no break intended
		case Node::Type::VECTOR_SCALAR_MULTIPLICATION: // no break intended
		case Node::Type::VECTOR_CONTRACTION: // no break intended
		case Node::Type::VECTOR_COMPARISON_IS_SMALLER: // no break intended
		case Node::Type::OUTPUT:
			break; // create instruction
		}

		// Create function identifier
		std::string fctId;
		GenerateInstructionId(&fctId, node.id);

		fileInstructionsH_.PrintfLine("void %s();", fctId.c_str());

		fileInstructions_.PrintfLine("void %s()", fctId.c_str());
		fileInstructions_.PrintfLine("{");
		fileInstructions_.Indent();

		retFalseOnFalse(GenerateOperationCode(
				&node,
				&fileInstructions_),
				"Could not generate Operation Code!\n");

		// End function
		fileInstructions_.Outdent();
		fileInstructions_.PrintfLine("}\n");

		// Add node to "nodes with instruction"
		nodesInstructionMap_.insert(std::pair<Node::Id_t, const Node*>(node.id, &node));

		// Determine Nodes array positions
		nodeArrayPos_.insert(std::pair<Node::Id_t, uint32_t>(node.id, arrayPos));
		arrayPos++;
	}

	return true;
}

bool CodeGenerator::GenerateCallbackPtCheck(FileWriter* file) const
{
	auto nodes = graph_->GetNodes();
	for(const Node &node: *nodes)
	{
		if(Node::Type::OUTPUT != node.type)
		{
			continue;
		}

		auto output = (const Interface::Output*) node.object;

		fprintProtect(file->PrintfLine("if(NULL == DacOutputCallback%s)",
				output->GetOutputName()->c_str()));
		fprintProtect(file->PrintfLine("{"));
		fprintProtect(file->PrintfLine("\tfatal(\"DacOutputCallback%s == NULL\");",
				output->GetOutputName()->c_str()));
		fprintProtect(file->PrintfLine("}\n"));
	}

	return true;
}

bool CodeGenerator::GenerateInstructionId(std::string * InstrId, const Node::Id_t NodeId)
{
	if(nullptr == InstrId)
	{
		Error("nullptr");
		return false;
	}

	*InstrId = "Node";
	*InstrId += std::to_string(NodeId);
	*InstrId += "Instruction";

	return true;
}

bool CodeGenerator::GenerateNodesElem(
		const Node::Id_t nodeId,
		const std::vector<uint32_t> * parentsArrayPosition,
		const std::vector<uint32_t> * childrenArrayPosition)
{
	std::string instrId;
	GenerateInstructionId(&instrId, nodeId);

	std::string buffer;
	buffer += instrId;
	buffer += ", {";

	if(NODE_T_MAX_EDGE_NUMBER < parentsArrayPosition->size())
	{
		Error("Too many parent nodes!");
		return false;
	}

	for(uint32_t parent = 0; parent < parentsArrayPosition->size(); parent++)
	{
		buffer += "&";
		buffer += nodesId;
		buffer += "[";
		buffer += std::to_string((*parentsArrayPosition)[parent]);
		buffer += "]";

		if(parentsArrayPosition->size() - 1 > parent)
		{
			buffer += ", ";
		}
	}

	buffer += "}, {";

	if(NODE_T_MAX_EDGE_NUMBER < childrenArrayPosition->size())
	{
		Error("Too many child nodes!");
		return false;
	}

	for(uint32_t child = 0; child < childrenArrayPosition->size(); child++)
	{
		buffer += "&";
		buffer += nodesId;
		buffer += "[";
		buffer += std::to_string((*childrenArrayPosition)[child]);
		buffer += "]";

		if(childrenArrayPosition->size() - 1 > child)
		{
			buffer += ", ";
		}
	}

	buffer += "}, 0, ";
	buffer += std::to_string(parentsArrayPosition->size());
	buffer += ", ";
	buffer += std::to_string(childrenArrayPosition->size());
	buffer += ", ";
	buffer += std::to_string(nodeId);

	fprintProtect(fileNodes_.PrintfLine("{%s},", buffer.c_str()));

	return true;
}

bool CodeGenerator::GenerateRunFunction()
{
	// Add prototype to header
	fprintProtect(fileDacH_.PrintfLine("extern int DacRun(void);"));

	// Define function
	fprintProtect(fileDacC_.PrintfLine("int DacRun(void)\n{"));
	fileDacC_.Indent();

	// Check that callbacks have been set
	GenerateCallbackPtCheck(&fileDacC_);

	// Fire up threads
	fprintProtect(fileDacC_.PrintfLine("StartThreads();"));

	// Join threads, create return values and closing brackets
	fprintProtect(fileDacC_.PrintfLine("JoinThreads();\n"));

	// Return 0 to show success.
	fprintProtect(fileDacC_.PrintfLine("return 0;\n}\n"));

	return true;
}

bool CodeGenerator::OutputCode(const Node* node, FileWriter * file)
{
	// Call corresponding function callbacks
	for(Node::Id_t outId: node->parents)
	{
		getVarRetFalseOnError(var, outId);

		auto output = (const Interface::Output*) node->object;

		const std::string * varIdentifier = var->GetIdentifier();
		if(nullptr == varIdentifier)
		{
			Error("Could not find Var. Identifier!\n");
			return false;
		}

		// If node is not an array, we need to take address
		std::string NodeStr;
		if(2 > var->Length())
		{
			NodeStr += "&";
		}
		NodeStr += *varIdentifier;

		fprintProtect(file->PrintfLine("DacOutputCallback%s(%s, sizeof(%s));\n",
				output->GetOutputName()->c_str(),
				NodeStr.c_str(),
				varIdentifier->c_str()));
	}

	fprintProtect(file->PrintfLine(""));

	return true;
}

bool CodeGenerator::GenerateOperationCode(const Node* node, FileWriter * file)
{
	switch(node->type)
	{
	case Node::Type::VECTOR_ADDITION:
		retFalseOnFalse(VectorAdditionCode(node, file), "Could not generate Vector Addition Code!\n");
		break;

	case Node::Type::CONTROL_TRANSFER_WHILE:
		retFalseOnFalse(ControlTransferWhileCode(node, file),
				"Could not generatede Control Transfer While Code!\n");
		break;

	case Node::Type::VECTOR_SCALAR_MULTIPLICATION:
		retFalseOnFalse(VectorScalarMultiplicationCode(node, file),
				"Could not generate Vector Scalar Multiplication Code!\n");
		break;

	case Node::Type::VECTOR_COMPARISON_IS_SMALLER:
		retFalseOnFalse(VectorComparisonIsSmallerCode(node, file),
				"Could not generate Vector Comparison <= Code!\n");
		break;

	case Node::Type::OUTPUT:
		retFalseOnFalse(OutputCode(node, file),
				"Could not generate Output Code!\n");
		break;

	case Node::Type::VECTOR:
		Error("Type %i is no operation!\n", (int) node->type);
		return false;

	case Node::Type::VECTOR_CONTRACTION:
		retFalseOnFalse(VectorContractionCode(node, file),
		"Could not generate vector contraction Code!\n");
		break;

	default:
		Error("Unknown Type %i!\n", (int) node->type);
		return false;
	}

	return true;
}

bool CodeGenerator::GenerateLocalVariableDeclaration(const Variable * var)
{
	if(!var->HasProperty(Variable::PROPERTY_GLOBAL))
	{
		std::string varDecl;
		var->GetDeclaration(&varDecl);
		varDecl += "\n";
		fprintProtect(fileDacC_.PrintfLine("\t%s", varDecl.c_str()));
	}

	return true;
}

bool CodeGenerator::VectorAdditionCode(const Node* node, FileWriter * file)
{
	getVarRetFalseOnError(varOp, node->id);
	getVarRetFalseOnError(varSum1, node->parents[0]);
	getVarRetFalseOnError(varSum2, node->parents[1]);

	GenerateLocalVariableDeclaration(varOp);

	auto vecOp = (const Algebra::Module::VectorSpace::Vector*) node->object;

	fprintProtect(file->PrintfLine("for(uint32_t dim = 0; dim < %u; dim++)",
			vecOp->__space_->GetDim()));

	fprintProtect(file->PrintfLine("{"));

	fprintProtect(file->PrintfLine("\t%s[dim] = %s[dim] + %s[dim];",
			varOp->GetIdentifier()->c_str(),
			varSum1->GetIdentifier()->c_str(),
			varSum2->GetIdentifier()->c_str()));

	fprintProtect(file->PrintfLine("}\n"));

	return true;
}

bool CodeGenerator::VectorContractionCode(const Node* node, FileWriter * file)
{
	getVarRetFalseOnError(varOp, node->id);
	getVarRetFalseOnError(varLVec, node->parents[0]);
	getVarRetFalseOnError(varRVec, node->parents[1]);

	const auto lnode = nodeMap_.find(node->parents[0]);
	if(nodeMap_.end() == lnode)
	{
		Error("Could not find Node for id %u\n", node->parents[0]);
		return false;
	}

	const Algebra::Module::VectorSpace::Vector* lVec = (const Algebra::Module::VectorSpace::Vector*) lnode->second->object;

	const auto rnode = nodeMap_.find(node->parents[1]);
	if(nodeMap_.end() == rnode)
	{
		Error("Could not find Node for id %u\n", node->parents[1]);
		return false;
	}

	const Algebra::Module::VectorSpace::Vector* rVec = (const Algebra::Module::VectorSpace::Vector*) rnode->second->object;

	const Algebra::Module::VectorSpace::Vector* opVec = (const Algebra::Module::VectorSpace::Vector*) node->object;
	const Algebra::Module::VectorSpace::Vector::contractValue_t * contractValue = (const Algebra::Module::VectorSpace::Vector::contractValue_t *) opVec->__operationParameters_;

	// Calculate Strides, assume Row-Major Layout
	// https://en.wikipedia.org/wiki/Row-_and_column-major_order#Address_calculation_in_general
	std::vector<uint32_t> lStrides;
	lVec->__space_->GetStrides(&lStrides);
	const char lstridesId[] = "lStrides";
	fprintProtect(file->PrintfLine("const uint32_t %s[] = {", lstridesId));
	for(const uint32_t &stride: lStrides)
	{
		fprintProtect(file->PrintfLine("\t %u,", stride));
	}
	fprintProtect(file->PrintfLine("};"));

	std::vector<uint32_t> rStrides;
	lVec->__space_->GetStrides(&rStrides);
	const char rstridesId[] = "rStrides";
	fprintProtect(file->PrintfLine("const uint32_t %s[] = {", rstridesId));
	for(const uint32_t &stride: rStrides)
	{
		fprintProtect(file->PrintfLine("\t %u,", stride));
	}
	fprintProtect(file->PrintfLine("};"));

	std::vector<uint32_t> opStrides;
	opVec->__space_->GetStrides(&opStrides);
	const char opstridesId[] = "opStrides";
	fprintProtect(file->PrintfLine("const uint32_t %s[] = {", opstridesId));
	for(const uint32_t &stride: opStrides)
	{
		fprintProtect(file->PrintfLine("\t %u,", stride));
	}
	fprintProtect(file->PrintfLine("};\n"));

	const char * varOpId = varOp->GetIdentifier()->c_str();

	bool resultIsArray = false;
	if(1 < varOp->Length())
	{
		resultIsArray = true;
	}

	if(resultIsArray)
	{
		fprintProtect(file->PrintfLine("for(int opIndex = 0; opIndex < sizeof(%s) / sizeof(%s[0]); opIndex++)",
				varOpId, varOpId));
		fprintProtect(file->PrintfLine("{"));
		file->Indent();

		std::string opIndexTuple = "const uint32_t opIndexTuple[] = {";
		for(uint32_t stride = 0; stride < opStrides.size(); stride++)
		{
			if(0 == stride)
			{
				opIndexTuple += "opIndex / ";
				opIndexTuple += opstridesId;
				opIndexTuple += "[";
				opIndexTuple += std::to_string(stride);
				opIndexTuple += "]";
				opIndexTuple += ", ";
			}
			else
			{
				opIndexTuple += "(opIndex % opStrides[";
				opIndexTuple += std::to_string(stride - 1);
				opIndexTuple += " - 1]) / opStrides[";
				opIndexTuple += std::to_string(stride);
				opIndexTuple += ", ";
			}
		}
		opIndexTuple.erase(opIndexTuple.end() - 2, opIndexTuple.end()); // remove last ", "
		opIndexTuple += "};";

		fprintProtect(file->PrintfLine("%s", opIndexTuple.c_str()));
	}

	fprintProtect(file->PrintfLine("%s sum = 0;", varOp->GetTypeString()));
	fprintProtect(file->PrintfLine("for(int dim = 0; dim < %u; dim++)",
			lVec->__space_->factors_[contractValue->lfactor].dim_));
	fprintProtect(file->PrintfLine("{"));

	std::string lIndexTuple = "\tconst uint32_t lIndexTuple[] = {";
	for(uint32_t lDim = 0; lDim < lVec->__space_->factors_.size(); lDim++)
	{
		if(lDim != contractValue->lfactor)
		{
			lIndexTuple += "lIndexTuple[";
			lIndexTuple += std::to_string(lDim);
			lIndexTuple += "], ";
		}
		else
		{
			lIndexTuple += "dim, ";
		}
	}

	lIndexTuple.erase(lIndexTuple.end() - 2, lIndexTuple.end()); // remove last ", "
	lIndexTuple += "};";
	fprintProtect(file->PrintfLine(lIndexTuple.c_str()));

	std::string rIndexTuple = "\tconst uint32_t rIndexTuple[] = {";
	for(uint32_t rDim = 0; rDim < rVec->__space_->factors_.size(); rDim++)
	{
		if(rDim != contractValue->rfactor)
		{
			rIndexTuple += "rIndexTuple[";
			rIndexTuple += std::to_string(rDim + lVec->__space_->factors_.size() - 1);
			rIndexTuple += "], ";
		}
		else
		{
			rIndexTuple += "dim, ";
		}
	}

	rIndexTuple.erase(rIndexTuple.end() - 2, rIndexTuple.end()); // remove last ", "
	rIndexTuple += "};";
	fprintProtect(file->PrintfLine(rIndexTuple.c_str()));

	std::string sum = "\tsum += ";
	sum += *(varLVec->GetIdentifier());
	for(uint32_t lIndex = 0; lIndex < lVec->__space_->factors_.size(); lIndex++)
	{
		sum += "[lIndexTuple[" + std::to_string(lIndex) + "]]";
	}

	sum += " * ";

	sum += *(varRVec->GetIdentifier());
	for(uint32_t rIndex = 0; rIndex < lVec->__space_->factors_.size(); rIndex++)
	{
		sum += "[rIndexTuple[" + std::to_string(rIndex) + "]]";
	}

	sum += ";";
	fprintProtect(file->PrintfLine(sum.c_str()));

	fprintProtect(file->PrintfLine("}\n"));

	if(resultIsArray)
	{
		fprintProtect(file->PrintfLine("%s[opIndex] = sum;",
				varOpId));

		file->Outdent();
		fprintProtect(file->PrintfLine("};"));
	}
	else
	{
		fprintProtect(file->PrintfLine("%s = sum;", varOpId));
	}

	return true;
}

bool CodeGenerator::VectorComparisonIsSmallerCode(const Node* node, FileWriter * file)
{
	// TODO: Create extra Norm-Nodes for this.
	getVarRetFalseOnError(varOp, node->id);
	getVarRetFalseOnError(varLVec, node->parents[0]);
	getVarRetFalseOnError(varRVec, node->parents[1]);

	std::string lNormId;
	lNormId += *(varLVec->GetIdentifier());
	lNormId += "Norm";
	lNormId += std::to_string(varLVec->GetNewRunningNumber());
	fprintProtect(file->PrintfLine("%s %s = 0;",
			varLVec->GetTypeString(),
			lNormId.c_str()));

	std::string rNormId;
	rNormId += *(varRVec->GetIdentifier());
	rNormId += "Norm";
	rNormId += std::to_string(varRVec->GetNewRunningNumber());
	fprintProtect(file->PrintfLine("%s %s = 0;",
			varRVec->GetTypeString(),
			rNormId.c_str()));

	std::string lArrayElem;
	varLVec->GetElement(&lArrayElem, "dim");

	fprintProtect(file->PrintfLine("for(uint32_t dim = 0; dim < %u; dim++)",
			varLVec->Length()));
	fprintProtect(file->PrintfLine("{"));
	fprintProtect(file->PrintfLine("\t %s += %s * %s;",
			lNormId.c_str(),
			lArrayElem.c_str(), lArrayElem.c_str()));
	fprintProtect(file->PrintfLine("}\n"));

	std::string rArrayElem;
	varRVec->GetElement(&rArrayElem, "dim");

	fprintProtect(file->PrintfLine("for(uint32_t dim = 0; dim < %u; dim++)",
			varRVec->Length()));
	fprintProtect(file->PrintfLine("{"));
	fprintProtect(file->PrintfLine("\t %s += %s * %s;",
			rNormId.c_str(),
			rArrayElem.c_str(), rArrayElem.c_str()));
	fprintProtect(file->PrintfLine("}\n"));

	retFalseOnFalse(GenerateLocalVariableDeclaration(varOp), "Could not generate Var. Decl.\n");

	fprintProtect(file->PrintfLine("if(%s < %s)",
			lNormId.c_str(), rNormId.c_str()));
	fprintProtect(file->PrintfLine("{"));
	fprintProtect(file->PrintfLine("\t %s = 1;",
			varOp->GetIdentifier()->c_str()));
	fprintProtect(file->PrintfLine("}"));
	fprintProtect(file->PrintfLine("else"));
	fprintProtect(file->PrintfLine("{"));
	fprintProtect(file->PrintfLine("\t %s = 0;",
			varOp->GetIdentifier()->c_str()));
	fprintProtect(file->PrintfLine("}\n"));

	return true;
}

bool CodeGenerator::ControlTransferWhileCode(const Node* node, FileWriter * file)
{
	getVarRetFalseOnError(varCond, node->parents[0]);

	if(1 != varCond->Length())
	{
		Error("Control Transfer condition has more than one dim.!\n");
		return false;
	}

	const auto * ctWhile = (ControlTransfer::While*) node->object;

	const auto &arrayPosTrue = nodeArrayPos_.find(ctWhile->getTrueNode());
	if(nodeArrayPos_.end() == arrayPosTrue)
	{
		Error("Couldn't find array position for Node%u\n",
				ctWhile->getTrueNode());

		return false;
	}

	const auto &arrayPosFalse = nodeArrayPos_.find(ctWhile->getFalseNode());
	if(nodeArrayPos_.end() == arrayPosFalse)
	{
		Error("Couldn't find array position for Node%u\n",
				ctWhile->getTrueNode());

		return false;
	}

	fprintProtect(file->PrintfLine("if(%s)", varCond->GetIdentifier()->c_str()));
	fprintProtect(file->PrintfLine("{"));
	fprintProtect(file->PrintfLine("\taddPossiblyDeferredJob(&nodes[%u]);", arrayPosTrue->second));
	fprintProtect(file->PrintfLine("}"));
	fprintProtect(file->PrintfLine("else"));
	fprintProtect(file->PrintfLine("{"));
	fprintProtect(file->PrintfLine("\taddPossiblyDeferredJob(&nodes[%u]);", arrayPosFalse->second));
	fprintProtect(file->PrintfLine("}"));

	return true;
}

bool CodeGenerator::VectorScalarMultiplicationCode(const Node* node, FileWriter * file)
{
	getVarRetFalseOnError(varOp, node->id);
	getVarRetFalseOnError(varVec, node->parents[0]);
	getVarRetFalseOnError(varScalar, node->parents[1]);

	retFalseOnFalse(GenerateLocalVariableDeclaration(varOp), "Could not generate Var. Decl.\n");

	auto vecOp = (const Algebra::Module::VectorSpace::Vector*) node->object;

	fprintProtect(file->PrintfLine("for(uint32_t dim = 0; dim < %u; dim++)",
			vecOp->__space_->GetDim()));

	fprintProtect(file->PrintfLine("{"));

	fprintProtect(file->PrintfLine("\t%s[dim] = %s[dim] * %s;",
			varOp->GetIdentifier()->c_str(),
			varVec->GetIdentifier()->c_str(),
			varScalar->GetIdentifier()->c_str()));

	fprintProtect(file->PrintfLine("}\n"));

	return true;
}

bool CodeGenerator::GenerateConstantDeclarations()
{
	for(const std::pair<Node::Id_t, Variable> &varPair: variables_)
	{
		const Variable * var = &varPair.second;
		if(var->HasProperty(Variable::PROPERTY_GLOBAL) && var->HasProperty(Variable::PROPERTY_CONST))
		{
			std::string decl;
			retFalseOnFalse(var->GetDeclaration(&decl), "Could not get declaration!\n");

			fprintProtect(fileInstructions_.PrintfLine("%s", decl.c_str()));
		}
	}

	return true;
}

bool CodeGenerator::GenerateStaticVariableDeclarations()
{
	for(const std::pair<Node::Id_t, Variable> &varPair: variables_)
	{
		const Variable * var = &varPair.second;
		if(var->HasProperty(Variable::PROPERTY_CONST))
		{
			// Constants are handled in another function
			continue;
		}

		if(var->HasProperty(Variable::PROPERTY_GLOBAL) && var->HasProperty(Variable::PROPERTY_STATIC))
		{
			std::string decl;
			retFalseOnFalse(var->GetDeclaration(&decl), "Could not get declaration!\n");

			fprintProtect(fileInstructions_.PrintfLine("%s", decl.c_str()));
		}
	}

	return true;
}

Variable* CodeGenerator::GetVariable(Node::Id_t id)
{
	const auto nodeIt = nodeMap_.find(id);
	if(nodeMap_.end() == nodeIt)
	{
		Error("Could not find Node for id %u\n", id);
		return nullptr;
	}

	Node::Id_t storageNodeId;
	if(Node::ID_NONE != nodeIt->second->storedIn_)
	{
		storageNodeId = nodeIt->second->storedIn_;
	}
	else
	{
		storageNodeId = id;
	}

	auto varIt = variables_.find(storageNodeId);
	if(variables_.end() == varIt)
	{
		Error("Node %u does not have a variable!\n", storageNodeId);
		return nullptr;
	}

	return &varIt->second;
}

bool CodeGenerator::FetchVariables()
{
	// Fetch all variables
	auto nodes = graph_->GetNodes();
	for(const Node &node: *nodes)
	{
		if(Node::ID_NONE != node.storedIn_)
		{
			continue; // This node is not stored in its own variable
		}

		std::string identifier;
		Variable::properties_t properties = Variable::PROPERTY_NONE;
		Variable::Type type = Variable::Type::none;
		size_t length = 1;
		const void * value;

		char tmpIdStr[42];
		SNPRINTF(tmpIdStr, sizeof(tmpIdStr), "Node%u", node.id);
		identifier.append(tmpIdStr);

		switch(node.objectType)
		{
		case Node::ObjectType::MODULE_VECTORSPACE_VECTOR:
		{
			auto vector = (const Algebra::Module::VectorSpace::Vector*) node.object;
			value = vector->__value_;
			length = vector->__space_->GetDim();
			switch(vector->__space_->GetRing())
			{
			case Algebra::Ring::Float32:
				type = Variable::Type::float_;
				break;

			case Algebra::Ring::Int32:
				type = Variable::Type::int32_;
				break;

			case Algebra::Ring::None: // no break intended
			default:
				Error("Unknown Ring %u!\n", vector->__space_->GetRing());
				return false;
			}

			if(nullptr != vector->__value_)
			{
				properties = (Variable::properties_t) (
						properties |
						Variable::PROPERTY_GLOBAL |
						Variable::PROPERTY_STATIC |
						Variable::PROPERTY_CONST);
			}
		}
		break;

		case Node::ObjectType::NONE: // no break intended
		case Node::ObjectType::INTERFACE_OUTPUT: // no break intended
		case Node::ObjectType::CONTROL_TRANSFER_WHILE:
			// No variable to create.
			continue;

		default:
			Error("Unknown ObjectType!\n");
			return false;
		}

		if(variables_.end() != variables_.find(node.id))
		{
			Error("Variable already exists!\n");
			return false;
		}

		if(node.usedAsStorageBy_.size())
		{
			properties = (Variable::properties_t) (
					properties & ~Variable::PROPERTY_CONST);
		}

		variables_.insert(
				std::make_pair(
						node.id,
						Variable(&identifier, properties, type, length, value)));
	}

	// Identify Outputs and mark output variables as such
	for(const Node &node: *nodes)
	{
		if(Node::Type::OUTPUT != node.type)
		{
			continue;
		}

		// Find all variables of the output's parents, i.e. the nodes that
		// shall be output
		for(const Node &potparnode: *nodes)
		{
			for(const Node::Id_t &parentNodeId: node.parents)
			{
				if(parentNodeId != potparnode.id)
				{
					continue;
				}

				getVarRetFalseOnError(var, parentNodeId);
				var->AddProperty(Variable::PROPERTY_GLOBAL);
				var->AddProperty(Variable::PROPERTY_STATIC);
			}
		}
	}

	// TODO: Identify constant variables!

	// TODO: Making all variables global and static so threads may use them easily for now
	for(auto &var: variables_)
	{
		var.second.AddProperty(Variable::PROPERTY_GLOBAL);
		var.second.AddProperty(Variable::PROPERTY_STATIC);
	}

	return true;
}

bool CodeGenerator::GenerateOutputFunctions()
{
	std::string fctDefinitions;

	auto nodes = graph_->GetNodes();
	for(const Node &node: *nodes)
	{
		if(Node::Type::OUTPUT != node.type)
		{
			continue;
		}

		auto * output = (const Interface::Output* ) node.object;

		// Get Variable attached to node
		getVarRetFalseOnError(var, node.parents[0]);

		std::string fctPtTypeId = "DacOutputCallback";
		fctPtTypeId += *(output->GetOutputName());

		std::string callbackTypedef;
		callbackTypedef += "typedef void (*";
		callbackTypedef += fctPtTypeId;
		callbackTypedef += "_t)(";
		callbackTypedef += "const ";
		callbackTypedef += var->GetTypeString();
		callbackTypedef += "* pt, size_t size);";

		// Export function prototype
		fprintProtect(fileDacH_.PrintfLine("%s", callbackTypedef.c_str()));
		fprintProtect(fileDacH_.PrintfLine("extern void %s_Register(%s_t callback);",
				fctPtTypeId.c_str(),
				fctPtTypeId.c_str()));

		// Declare Static Variables keeping the callback pointers
		fprintProtect(fileDacC_.PrintfLine("%s_t %s = NULL;",
				fctPtTypeId.c_str(),
				fctPtTypeId.c_str()));

		fprintProtect(fileInstructions_.PrintfLine("extern %s_t %s;",
				fctPtTypeId.c_str(),
				fctPtTypeId.c_str()));

		// Define Function
		char tmpBuff[100];
		SNPRINTF(tmpBuff, sizeof(tmpBuff), "void %s_Register(%s_t callback)\n{\n",
				fctPtTypeId.c_str(),
				fctPtTypeId.c_str());

		fctDefinitions += tmpBuff;

		SNPRINTF(tmpBuff, sizeof(tmpBuff), "\t %s = callback;\n", fctPtTypeId.c_str());
		fctDefinitions += tmpBuff;
		fctDefinitions += "}\n\n";
	}

	fprintProtect(fileDacC_.PrintfLine(""));

	auto exportedHeading = std::string("Exported Functions");
	retFalseOnFalse(GenerateHeading(&exportedHeading), "Exported Heading failed!\n");

	fprintProtect(fileDacC_.PrintfLine("%s", fctDefinitions.c_str()));

	return true;
}

bool CodeGenerator::GenerateHeading(const std::string * heading)
{
	const uint16_t headingWidth = 100;

	std::string starSpangledHeading;
	starSpangledHeading.reserve(headingWidth);

	starSpangledHeading += "\n/* ";
	starSpangledHeading += *heading;
	starSpangledHeading += " ";

	while(headingWidth > starSpangledHeading.length())
	{
		starSpangledHeading += "*";
	}

	fprintProtect(fileDacC_.PrintfLine("%s*/", starSpangledHeading.c_str()));

	return true;
}

bool CodeGenerator::GenerateIncludes()
{
	for(uint16_t incl = 0; incl < sizeof(includeFilesBrackets) / sizeof(includeFilesBrackets[0]); incl++)
	{
		fprintProtect(fileDacC_.PrintfLine("#include <%s>", includeFilesBrackets[incl]));
	}

	for(uint16_t incl = 0; incl < sizeof(includeFilesQuotes) / sizeof(includeFilesQuotes[0]); incl++)
	{
		fprintProtect(fileDacC_.PrintfLine("#include \"%s\"", includeFilesQuotes[incl]));
	}

	return true;
}

inline bool Variable::HasProperty(properties_t property) const
{
	if(properties_ & property)
	{
		return true;
	}

	return false;
}

const char* Variable::GetTypeString() const
{
	static const char typeStrings[(int) Type::nrOf][30] = {
			"0000", // make sure it throws error
			"uint8_t",
			"int8_t",
			"int32_t",
			"float",
	};

	switch(type_)
	{
	case Type::uint8_:
		return typeStrings[(int) Type::uint8_];

	case Type::int8_:
		return typeStrings[(int) Type::int8_];

	case Type::int32_:
		return typeStrings[(int) Type::int32_];
		break;

	case Type::float_:
		return typeStrings[(int) Type::float_];

	default: // no break intended
	case Type::none:
		Error("Unknown Type %u!\n", (unsigned int) type_);
		return nullptr;
	}

	return nullptr; // should not be reached
}

bool Variable::AddProperty(properties_t property)
{
	switch(property)
	{
	case PROPERTY_CONST: // no break intended
	case PROPERTY_STATIC: // no break intended
	case PROPERTY_GLOBAL:
		break;

	default:
		Error("Unknown property!\n");
		return false;
	}

	properties_ = (properties_t) (properties_ | property);

	return true;
}

Variable::Variable(const std::string* identifier, properties_t properties, Type type, size_t length, const void* value)
{
	value_ = value;

	if(PROPERTY_END_OF_ENUM <= properties)
	{
		Error("Unknown properties specified!\n");
		return;
	}
	properties_ = properties;

	type_ = type;
	length_ = length;

	if(nullptr == identifier)
	{
		Error("Nullpointer!\n");
		return;
	}

	identifier_ = *identifier;
}

bool Variable::GetDeclaration(std::string* decl) const
{
	if(properties_ & PROPERTY_STATIC)
	{
		decl->append("static ");
	}

	if(properties_ & PROPERTY_CONST)
	{
		decl->append("const ");
	}

	const char* typeStr = GetTypeString();
	if(nullptr == typeStr)
	{
		Error("Unknown type!\n");
		return false;
	}

	decl->append(typeStr);
	decl->append(" ");

	if(0 == identifier_.length())
	{
		Error("Identifier not set!\n");
		return false;
	}

	decl->append(identifier_);

	if(1 < length_) // this is an array
	{
		char tmpBuff[40];
		SNPRINTF(tmpBuff, sizeof(tmpBuff), "[%lu]", length_);
		decl->append(tmpBuff);
	}

	// Any initializer supplied?
	if(properties_ & PROPERTY_CONST)
	{
		if(nullptr == value_)
		{
			Error("Const variable requires initializer!");
			return false;
		}
	}

	if(nullptr == value_)
	{
		decl->append(";");
		return true;
	}

	decl->append(" = ");

	if(1 < length_)
	{
		decl->append("{");
	}

	for(uint32_t elem = 0; elem < length_; elem++)
	{
		char tmpBuff[42];
		switch(type_)
		{
		case Type::uint8_: // no break intended
		case Type::int8_:
			Error("Type not implemented!\n");
			return false;

		case Type::float_:
		{
			float* valuePt = (float*) value_;
			SNPRINTF(tmpBuff, sizeof(tmpBuff), "%f", valuePt[elem]);
		}
		break;

		case Type::int32_:
		{
			int32_t* valuePt = (int32_t*) value_;
			SNPRINTF(tmpBuff, sizeof(tmpBuff), "%i", valuePt[elem]);
		}
		break;

		default:
			Error("Unknown Type\n");
			return false;
		}

		decl->append(tmpBuff);

		if(elem != length_ -1)
		{
			decl->append(", ");
		}
	}

	if(1 < length_)
	{
		decl->append("}");
	}

	decl->append(";");

	return true;
}

const std::string* Variable::GetIdentifier() const
{
	if(0 == identifier_.length())
	{
		Error("Identifier not set!\n");
		return nullptr;
	}

	return &identifier_;
}

uint32_t Variable:: GetNewRunningNumber()
{
	runningNumber_++;
	return runningNumber_;
}

bool Variable::GetElement(std::string* elem, const char * elemIndex) const
{
	if(1 >= length_)
	{
		Error("Variable %s not an Array!\n", identifier_.c_str());
		return false;
	}

	if(0 == identifier_.length())
	{
		Error("Identifier not set!\n");
		return false;
	}

	elem->append(identifier_);

	char tmpBuff[40];
	SNPRINTF(tmpBuff, sizeof(tmpBuff), "[%s]", elemIndex);
	elem->append(tmpBuff);

	return true;
}

size_t Variable::Length() const
{
	return length_;
}


