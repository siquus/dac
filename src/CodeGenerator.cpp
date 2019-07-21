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

#include "GlobalDefines.h"
#include "Ring.h"
#include "Interface.h"
#include "embeddedFiles.h"

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

#define retFalseIfNotFound(pair, map, key) \
	const auto pair = map.find(key); \
	if(map.end() == pair) \
	{ \
		Error("Unknown Key!\n"); \
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

CodeGenerator::CodeGenerator(const std::string* path) {
	path_ = *path;
}

CodeGenerator::~CodeGenerator() {
	if(nullptr != outfile_)
	{
		if(fclose(outfile_))
		{
			Error("fclose failed: %s\n", strerror(errno));
		}
	}

	if(nullptr != outHeaderFile_)
	{
		if(fclose(outHeaderFile_))
		{
			Error("fclose failed: %s\n", strerror(errno));
		}
	}

	for(const auto &thread: cpuThreads_)
	{
		if(nullptr != thread.fileDes)
		{
			if(fclose(thread.fileDes))
			{
				Error("fclose failed: %s\n", strerror(errno));
			}
		}
	}
}

bool CodeGenerator::Generate(const Graph* graph, const Parallizer* parallizer)
{
	graph_ = graph;

	// From here on we assume that the graph will no longer change
	// this means we may use pointers to nodes now!
	auto nodes = graph_->GetNodes();
	for(const Graph::Node_t &node: *nodes)
	{
		nodeMap_.insert(std::make_pair(node.id, &node));
	}

	for(const auto &nodePair: nodeMap_)
	{
		DEBUG("nodeMap Node%u: NodeType %u, ObjType %u,  Children ",
				nodePair.first,
				(unsigned int) nodePair.second->nodeType,
				(unsigned int) nodePair.second->objectType);

		for(Graph::NodeId_t nodeId: nodePair.second->children)
		{
			DEBUG("%u, ", nodeId);
		}
		DEBUG("Parents ");
		for(Graph::NodeId_t nodeId: nodePair.second->parents)
		{
			DEBUG("%u, ", nodeId);
		}
		DEBUG("\n");
	}

	retFalseOnFalse(FetchVariables(), "Could not fetch variables\n");

	std::string pathAndFileName = path_ + fileName;

	outfile_ = fopen((pathAndFileName + ".c").c_str(), "w");
	if(nullptr == outfile_)
	{
		Error("Open File %s failed: %s\n", (pathAndFileName + ".c").c_str(), strerror(errno));
		return false;
	}

	outHeaderFile_ = fopen((pathAndFileName + ".h").c_str(), "w");
	if(nullptr == outfile_)
	{
		Error("Open File %s failed: %s\n", (pathAndFileName + ".h").c_str(), strerror(errno));
		return false;
	}

	// Copy files
	GenerateEmbeddedFiles(&path_);

	// Create threads
	auto cpuInfo = parallizer->GetCpuInfo();
	if(1 != cpuInfo->size())
	{
		Error("Only one CPU supported currently!\n");
		return false;
	}

	const Parallizer::cpu_t * cpu = &cpuInfo->begin()->second;
	for(uint16_t thread = 0; thread < cpu->coresNrOf; thread++)
	{
		cpuThread_t cpuThread;
		SNPRINTF(cpuThread.filePath, sizeof(cpuThread.filePath), "%sthread%u.h", path_.c_str(), thread);
		SNPRINTF(cpuThread.pthread, sizeof(cpuThread.pthread), "pthread%u", thread);

		cpuThread.fileDes = fopen(cpuThread.filePath, "w");
		if(nullptr == cpuThread.fileDes)
		{
			Error("Open File %s failed: %s\n", cpuThread.filePath, strerror(errno));
			return false;
		}

		cpuThreads_.push_back(cpuThread);
	}

	// Create headers
	fprintProtect(fprintf(outfile_, "%s\n", fileHeader));
	fprintProtect(fprintf(outHeaderFile_, "%s\n", fileHeader));

	for(const auto &thread: cpuThreads_)
	{
			fprintProtect(fprintf(thread.fileDes, "%s\n", fileHeader));
	}

	// Generate Includes
	retFalseOnFalse(GenerateIncludes(), "Could not generate Includes\n");

	// Generate global variables
	auto constHeading = std::string("Constant Variables");
	retFalseOnFalse(GenerateHeading(&constHeading), "Constant Heading failed!\n");

	retFalseOnFalse(GenerateConstantDeclarations(), "Could not generate Constants\n");

	auto staticHeading = std::string("Static Variables");
	retFalseOnFalse(GenerateHeading(&staticHeading), "Static Heading failed!\n");

	for(const auto &thread: cpuThreads_)
	{
		fprintProtect(fprintf(outfile_, "static pthread_t %s;\n", thread.pthread));
	}
	fprintProtect(fprintf(outfile_, "\n"));

	retFalseOnFalse(GenerateThreadSynchVariables(), "Could not generate Thread Synch Variables!\n");

	retFalseOnFalse(GenerateStaticVariableDeclarations(), "Could not generate Statics\n!");

	// Generate Functions
	retFalseOnFalse(GenerateOutputFunctions(), "Could not generate Output Functions\n!");

	auto threadIncludeHeading = std::string("Thread functions defined inside header files");
	retFalseOnFalse(GenerateHeading(&threadIncludeHeading), "Thread Include Heading failed!\n");

	retFalseOnFalse(GenerateThreadIncludes(), "Could not generate Thread Includes\n!");

	auto runHeading = std::string("The main run routine");
	retFalseOnFalse(GenerateHeading(&runHeading), "Run Heading failed!\n");

	retFalseOnFalse(GenerateRunFunction(), "Could not generate Run Function!\n");

	return true;
}

bool CodeGenerator::GenerateThreadIncludes()
{
	for(const auto &thread: cpuThreads_)
	{
		fprintProtect(fprintf(outfile_, "#include \"%s\"\n", thread.filePath));
	}

	fprintProtect(fprintf(outfile_, "\n"));

	return true;
}

bool CodeGenerator::GenerateRunFunction()
{
	// Add prototype to header
	fprintProtect(fprintf(outHeaderFile_, "extern int DacRun(void);\n"));

	// Define function
	fprintProtect(fprintf(outfile_, "int DacRun(void)\n{\n"));

	// Traverse the Graph and generate Code
	// Find all nodes which do not have parents and create a set of their children
	std::set<Graph::NodeId_t> roots;
	std::set<Graph::NodeId_t> firstGenerationChildren;

	auto nodes = graph_->GetNodes();
	for(const Graph::Node_t &node: *nodes)
	{
		if(0 == node.parents.size())
		{
			roots.insert(node.id);
			std::copy(
					node.children.begin(), node.children.end(),
					std::inserter(firstGenerationChildren, firstGenerationChildren.end()));
		}
	}

	// delete all children which have non-root parents.
	// i.e. these children may be executed right away.
	std::vector<Graph::NodeId_t> nonFirstGenerationChildren;
	for(Graph::NodeId_t childId: firstGenerationChildren)
	{
		const auto childNodeIt =  nodeMap_.find(childId);
		if(nodeMap_.end() == childNodeIt)
		{
			Error("Unknown Child, NodeId = %u!\n", childId);
			return false;
		}

		const Graph::Node_t* childNode = childNodeIt->second;

		for(Graph::NodeId_t parentId: childNode->parents)
		{
			if(roots.end() == roots.find(parentId))
			{
				// Child has one non-first-generation parent.
				nonFirstGenerationChildren.push_back(childId); // erase later
				break;
			}
		}
	}

	for(Graph::NodeId_t childId: nonFirstGenerationChildren)
	{
		firstGenerationChildren.erase(childId);
	}

	std::set<Graph::NodeId_t> * children = &firstGenerationChildren;

	std::set<Graph::NodeId_t> tmpSet;
	std::set<Graph::NodeId_t> * nextGenChildren = &tmpSet;

	while(1)
	{
		// Create Code for all children and create new set with the next generation
		for(Graph::NodeId_t childId: *children)
		{
			retFalseIfNotFound(nodePair, nodeMap_, childId);

			retFalseOnFalse(GenerateOperationCode(nodePair->second, outfile_), "Could not generate Operation Code!\n");
			std::copy(
					nodePair->second->children.begin(), nodePair->second->children.end(),
								std::inserter(*nextGenChildren, nextGenChildren->end()));

			generatedNodes_.insert(childId);
		}

		// Remove all nextGen children who's parents have not been generated
		std::vector<Graph::NodeId_t> childrenToBeRemoved;
		for(Graph::NodeId_t childId: *nextGenChildren)
		{
			retFalseIfNotFound(childPair, nodeMap_, childId);
			for(Graph::NodeId_t parentId: childPair->second->parents)
			{
				if(generatedNodes_.end() == generatedNodes_.find(parentId))
				{
					childrenToBeRemoved.push_back(childId);
					break;
				}
			}
		}

		for(Graph::NodeId_t removeId: childrenToBeRemoved)
		{
			nextGenChildren->erase(removeId);
		}

		// Any children left to generate?
		if(0 == nextGenChildren->size())
		{
			break;
		}

		// Prepare sets for next iteration
		children->clear();
		auto tmp = children;
		children = nextGenChildren;
		nextGenChildren = tmp;
	}

	// Return 0 to show success.
	fprintProtect(fprintf(outfile_, "\treturn 0;\n}\n\n"));

	return true;
}

bool CodeGenerator::OutputCode(const Graph::Node_t* node, FILE* file)
{
	// Call corresponding function callbacks
	for(Graph::NodeId_t outId: node->parents)
	{
		auto output = (const Interface::Output*) node->object;

		fprintProtect(fprintf(file, "\tDacOutputCallback%s(Node%u, sizeof(Node%u));\n",
				output->GetOutputName(outId)->c_str(),
				outId, outId));
	}

	fprintProtect(fprintf(file, "\n"));

	return true;
}

bool CodeGenerator::GenerateOperationCode(const Graph::Node_t* node, FILE* file)
{
	switch(node->nodeType)
	{
	case Graph::NodeType::VECTOR_ADDITION:
		retFalseOnFalse(VectorAdditionCode(node, file), "Could not generate Vector Addition Code!\n");
		break;

	case Graph::NodeType::VECTOR_SCALAR_MULTIPLICATION:
		retFalseOnFalse(VectorScalarMultiplicationCode(node, file),
				"Could not generate Vector Scalar Multiplication Code!\n");
		break;

	case Graph::NodeType::OUTPUT:
		retFalseOnFalse(OutputCode(node, file),
				"Could not generate Output Code!\n");
		break;

	case Graph::NodeType::VECTOR:
		Error("NodeType %i is no operation!\n", (int) node->nodeType);
		return false;

	default:
		Error("Unknown NodeType %i!\n", (int) node->nodeType);
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
		fprintProtect(fprintf(outfile_, "\t%s\n", varDecl.c_str()));
	}

	return true;
}

bool CodeGenerator::VectorAdditionCode(const Graph::Node_t* node, FILE* file)
{
	retFalseIfNotFound(varOp, variables_, node->id);
	retFalseIfNotFound(varSum1, variables_, node->parents[0]);
	retFalseIfNotFound(varSum2, variables_, node->parents[1]);

	GenerateLocalVariableDeclaration(&varOp->second);

	auto vecOp = (const Algebra::Module::VectorSpace::Vector*) node->object;

	fprintProtect(fprintf(file, "\tfor(uint32_t dim = 0; dim < %u; dim++)\n\t{\n",
			vecOp->__space_->dim_));

	fprintProtect(fprintf(file, "\t\t%s[dim] = %s[dim] + %s[dim];\n\t}\n\n",
			varOp->second.GetIdentifier()->c_str(),
			varSum1->second.GetIdentifier()->c_str(),
			varSum2->second.GetIdentifier()->c_str()));

	return true;
}

bool CodeGenerator::VectorScalarMultiplicationCode(const Graph::Node_t* node, FILE* file)
{
	retFalseIfNotFound(varOp, variables_, node->id);
	retFalseIfNotFound(varVec, variables_, node->parents[0]);
	retFalseIfNotFound(varScalar, variables_, node->parents[1]);

	GenerateLocalVariableDeclaration(&varOp->second);

	auto vecOp = (const Algebra::Module::VectorSpace::Vector*) node->object;

	fprintProtect(fprintf(file, "\tfor(uint32_t dim = 0; dim < %u; dim++)\n\t{\n",
			vecOp->__space_->dim_));

	fprintProtect(fprintf(file, "\t\t%s[dim] = %s[dim] * %s;\n\t}\n\n",
			varOp->second.GetIdentifier()->c_str(),
			varVec->second.GetIdentifier()->c_str(),
			varScalar->second.GetIdentifier()->c_str()));

	return true;
}

bool CodeGenerator::GenerateConstantDeclarations()
{
	for(const std::pair<Graph::NodeId_t, Variable> &varPair: variables_)
	{
		const Variable * var = &varPair.second;
		if(var->HasProperty(Variable::PROPERTY_GLOBAL) && var->HasProperty(Variable::PROPERTY_CONST))
		{
			std::string decl;
			retFalseOnFalse(var->GetDeclaration(&decl), "Could not get declaration!\n");

			fprintProtect(fprintf(outfile_, "%s\n", decl.c_str()));

			generatedNodes_.insert(varPair.first);
		}
	}

	return true;
}

bool CodeGenerator::GenerateThreadSynchVariables()
{
	std::string tmpString;
	for(const auto &var: variables_)
	{
		if(var.second.HasProperty(Variable::PROPERTY_CONST))
		{
			// Constant variables require no mutex
			continue;
		}

		var.second.GetMutexIdentifier(&tmpString);
		fprintProtect(fprintf(outfile_, "static pthread_mutex_t %s = PTHREAD_MUTEX_INITIALIZER;\n",
				tmpString.c_str()));

		var.second.GetConditionIdentifier(&tmpString);
		fprintProtect(fprintf(outfile_, "static pthread_cond_t %s = PTHREAD_COND_INITIALIZER;\n",
				tmpString.c_str()));

		var.second.GetReadyIdentifier(&tmpString);
		fprintProtect(fprintf(outfile_, "static uint8_t %s = 0;\n\n",
				tmpString.c_str()));
	}

	return true;
}

bool CodeGenerator::GenerateStaticVariableDeclarations()
{
	for(const std::pair<Graph::NodeId_t, Variable> &varPair: variables_)
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

			fprintProtect(fprintf(outfile_, "%s\n", decl.c_str()));

			generatedNodes_.insert(varPair.first);
		}
	}

	return true;
}

bool CodeGenerator::FetchVariables()
{
	// Fetch all variables
	auto nodes = graph_->GetNodes();
	for(const Graph::Node_t &node: *nodes)
	{
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
		case Graph::ObjectType::MODULE_VECTORSPACE_VECTOR:
		{
			auto vector = (const Algebra::Module::VectorSpace::Vector*) node.object;
			value = vector->__value_;
			length = vector->__space_->dim_;
			switch(vector->__space_->ring_)
			{
			case Algebra::Ring::Type::Float32:
				type = Variable::Type::float_;
				break;

			case Algebra::Ring::Type::None: // no break intended
			default:
				Error("Unknown Ring!\n");
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

		case Graph::ObjectType::INTERFACE_OUTPUT:
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

		variables_.insert(
				std::make_pair(
						node.id,
						Variable(&identifier, properties, type, length, value)));
	}

	// Identify Outputs and mark output variables as such
	for(const Graph::Node_t &node: *nodes)
	{
		if(Graph::NodeType::OUTPUT != node.nodeType)
		{
			continue;
		}

		// Find all variables of the output's parents, i.e. the nodes that
		// shall be output
		for(const Graph::Node_t &potparnode: *nodes)
		{
			for(const Graph::NodeId_t &parentNodeId: node.parents)
			{
				if(parentNodeId != potparnode.id)
				{
					continue;
				}

				// Found output parent variable
				auto var = variables_.find(parentNodeId);
				if(variables_.end() == var)
				{
					Error("Parent Variable does not exists!\n");
					return false;
				}

				var->second.AddProperty(Variable::PROPERTY_GLOBAL);
				var->second.AddProperty(Variable::PROPERTY_STATIC);
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
	for(const Graph::Node_t &node: *nodes)
	{
		if(Graph::NodeType::OUTPUT != node.nodeType)
		{
			continue;
		}

		auto * output = (const Interface::Output* ) node.object;

		for(const Graph::NodeId_t &nodeId: node.parents)
		{
			// Get Variable attached to node
			const auto var = variables_.find(nodeId);
			if(variables_.end() == var)
			{
				Error("Variable does not exist!\n");
				return false;
			}

			std::string fctPtTypeId = "DacOutputCallback";
			fctPtTypeId += *(output->GetOutputName(nodeId));

			std::string callbackTypedef;
			callbackTypedef += "typedef void (*";
			callbackTypedef += fctPtTypeId;
			callbackTypedef += "_t)(";
			callbackTypedef += var->second.GetTypeString();
			callbackTypedef += "* pt, size_t size);";

			// Export function prototype
			fprintProtect(fprintf(outHeaderFile_, "%s\n", callbackTypedef.c_str()));
			fprintProtect(fprintf(outHeaderFile_, "extern void %s_Register(%s_t callback);\n",
					fctPtTypeId.c_str(),
					fctPtTypeId.c_str()));

			// Declare Static Variables keeping the callback pointers
			fprintProtect(fprintf(outfile_, "static %s_t %s = NULL;",
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
	}

	fprintProtect(fprintf(outfile_, "\n"));

	auto exportedHeading = std::string("Exported Functions");
	retFalseOnFalse(GenerateHeading(&exportedHeading), "Exported Heading failed!\n");

	fprintProtect(fprintf(outfile_, "%s\n", fctDefinitions.c_str()));

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

	fprintProtect(fprintf(outfile_, "%s*/\n", starSpangledHeading.c_str()));

	return true;
}

bool CodeGenerator::GenerateIncludes()
{
	if(nullptr == outfile_)
	{
		Error("Output file is not open!\n");
		return false;
	}

	for(uint16_t incl = 0; incl < sizeof(includeFilesBrackets) / sizeof(includeFilesBrackets[0]); incl++)
	{
		fprintProtect(fprintf(outfile_, "#include <%s>\n", includeFilesBrackets[incl]));
	}

	for(uint16_t incl = 0; incl < sizeof(includeFilesQuotes) / sizeof(includeFilesQuotes[0]); incl++)
	{
		fprintProtect(fprintf(outfile_, "#include \"%s\"\n", includeFilesQuotes[incl]));
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

void Variable::GetMutexIdentifier(std::string * mutex) const
{
	*mutex = "isReady";
	*mutex += identifier_;
	*mutex += "Mutex";

	return;
}

void Variable::GetConditionIdentifier(std::string * condId) const
{
	*condId = "isReady";
	*condId += identifier_;
	*condId += "Condition";

	return;
}

void Variable::GetReadyIdentifier(std::string * readyId) const
{
	*readyId = "isReady";
	*readyId += identifier_;

	return;
}

const char* Variable::GetTypeString() const
{
	static const char typeStrings[(int) Type::nrOf][30] = {
			"0000", // make sure it throws error
			"uint8_t",
			"int8_t",
			"float",
	};

	switch(type_)
	{
	case Type::uint8_:
		return typeStrings[(int) Type::uint8_];

	case Type::int8_:
		return typeStrings[(int) Type::int8_];

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


