/*
 * This file is part of
 * Distributed Algebraic Computations (https://github.com/siquus/dac)
 *
 * GPL-3 (or later)
 *
 * Copyright (C) 2020  Patrik Omland
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SRC_CODEGENERATOR_H_
#define SRC_CODEGENERATOR_H_

#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <memory>
#include <set>

#include "Graph.h"
#include "Module.h"

class FileWriter {
	std::string path_;
	FILE * outfile_ = nullptr;
	uint8_t indentationLevel_ = 0;

public:
	bool Init(const std::string* path);
	~FileWriter();
	void PrintfLine(const char * format, ...);
	void Indent(uint8_t tabNubmer = 1);
	void Outdent(uint8_t tabNumber = 1);
	const std::string* Path() const;
};

class Variable {
public:
	typedef enum {
		PROPERTY_NONE = 0,
		PROPERTY_CONST = 	1 << 0,
		PROPERTY_STATIC = 	1 << 1,
		PROPERTY_GLOBAL =	1 << 2,
		PROPERTY_POINTER = 1 << 3,
		PROPERTY_END_OF_ENUM = 1 << 4,
	} properties_t;

	enum class Type {
		none,
		uint8_,
		int8_,
		int32_,
		float_,
		nrOf,
	};

	Variable(const std::string* identifier, properties_t properties, Type type, size_t length = 1, const void* value = nullptr);

	bool GetDeclaration(std::string* decl) const;
	const std::string * GetIdentifier() const;
	bool GetElement(std::string* elem,  const char *elemIndex) const;
	size_t Length() const;
	bool HasProperty(properties_t property) const;
	bool AddProperty(properties_t property);
	Type GetType() const;
	const char* GetTypeString() const;
	uint32_t GetNewRunningNumber();

private:
	properties_t properties_;
	Type type_;
	size_t length_;
	std::string identifier_;
	const void* value_;
	uint32_t runningNumber_ = 0; // To make declarations unique
};

class CodeGenerator {
	std::string path_;

	FileWriter fileDacC_;
	FileWriter fileDacH_;
	FileWriter fileInstructions_;
	FileWriter fileInstructionsH_;

	const Graph* graph_ = nullptr;

	bool GenerateConstants();
	bool GenerateStatics();
	bool GenerateIncludes();
	bool GenerateHeading(const std::string * heading);
	bool GenerateInterfaceFunctions();
	bool GenerateConstantDeclarations();
	bool GenerateStaticVariableDeclarations();
	bool GenerateLocalVariableDeclaration(const Variable * var);
	bool GenerateRunFunction();
	bool GenerateInstructions();
	bool GenerateNodesArray();
	bool GenerateInstructionId(std::string * instrId, const Node::Id_t nodeId);
	bool GenerateNodesElem(
			const Node::Id_t nodeId,
			const std::vector<Node::Id_t> * parents,
			const std::vector<Node::Id_t> * children);

	bool GenerateOperationCode(const Node* node, FileWriter * file);
	bool OutputCode(const Node* node, FileWriter * file);
	bool InputCode(const Node* node, FileWriter * file);
	bool GenerateCallbackPtCheck(FileWriter* file) const;
	bool VectorAdditionCode(const Node* node, FileWriter* file);
	bool VectorScalarProductCode(const Node* node, FileWriter * file, bool divide = false);
	bool VectorScalarProductKroneckerDeltaCode(const Node* node, FileWriter * file, bool divide = false);
	bool VectorVectorProductCode(const Node* node, FileWriter * file, bool divide = false);
	bool VectorPowerCode(const Node* node, FileWriter * file);
	bool VectorVectorProductKroneckerDeltaCode(const Node* node, FileWriter * file, bool divide = false);
	bool VectorComparisonIsSmallerCode(const Node* node, FileWriter * file);
	bool VectorContractionCode(const Node* node, FileWriter * file);
	bool VectorContractionKroneckerDeltaCode(const Node* node, FileWriter * file);
	bool ControlTransferWhileCode(const Node* node, FileWriter * file);
	bool VectorPermutationCode(const Node* node, FileWriter * file);
	bool VectorProjectionCode(const Node* node, FileWriter * file);
	bool VectorJoinIndicesCode(const Node* node, FileWriter * file);
	bool VectorIndexSplitSumCode(const Node* node, FileWriter * file);
	bool VectorCrossCorrelationCode(const Node* node, FileWriter * file);
	bool VectorMaxPoolCode(const Node* node, FileWriter * file);

	bool FetchVariables();
	bool GetFirstNodesToExecute(std::set<Node::Id_t> * nodeSet);

	bool GetRootAncestorInstructionPositions(std::set<uint32_t> * instructionPos, Node::Id_t child);

	std::map<Node::Id_t, Variable> variables_;
	Variable* GetVariable(Node::Id_t id);

	std::map<Node::Id_t, const Node*> nodesInstructionMap_;
	std::map<Node::Id_t, uint32_t> nodeArrayPos_;

	size_t ThreadsNrOf_;

public:
	CodeGenerator(const std::string* path);
	virtual ~CodeGenerator();

	bool Generate(const Graph* graph);
};

#endif /* SRC_CODEGENERATOR_H_ */
