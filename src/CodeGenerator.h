/*
 * CodeGenerator.h
 *
 *  Created on: May 22, 2019
 *      Author: derommo
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
#include "Parallizer.h"

class FileWriter {
	std::string path_;
	FILE * outfile_ = nullptr;
	uint8_t indentationLevel_ = 0;

public:
	bool Init(const std::string* path);
	~FileWriter();
	int PrintfLine(const char * format, ...);
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
		PROPERTY_END_OF_ENUM = 1 << 3,
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
	const char* GetTypeString() const;
	void GetMutexIdentifier(std::string * mutex) const;
	void GetReadyIdentifier(std::string * readyId) const;
	void GetConditionIdentifier(std::string * condId) const;
	bool GenerateLock(FileWriter * file);
	bool GenerateUnlock(FileWriter * file);
	bool GenerateConditionWait(FileWriter * file, const std::string* iteration);
	bool GenerateConditionIncrement(FileWriter * file) const;
	bool GenerateConditionBroadcast(FileWriter * file);
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
	FileWriter fileNodes_;
	FileWriter fileInstructions_;

	const Graph* graph_ = nullptr;

	bool GenerateConstants();
	bool GenerateStatics();
	bool GenerateIncludes();
	bool GenerateHeading(const std::string * heading);
	bool GenerateOutputFunctions();
	bool GenerateConstantDeclarations();
	bool GenerateStaticVariableDeclarations();
	bool GenerateThreadSynchVariables();
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
	bool GenerateCallbackPtCheck(FileWriter* file) const;
	bool VectorAdditionCode(const Node* node, FileWriter* file);
	bool VectorScalarMultiplicationCode(const Node* node, FileWriter * file);
	bool VectorComparisonIsSmallerCode(const Node* node, FileWriter * file);

	bool FetchVariables();

	std::map<Node::Id_t, Variable> variables_;
	std::map<Node::Id_t, const Node*> nodeMap_;
	std::set<Node::Id_t> generatedNodes_;
	std::map<Node::Id_t, const Node*> nodesInstructionMap_;

	typedef struct {
		char pthread[42];
	} cpuThread_t;

	std::vector<cpuThread_t> cpuThreads_;

public:
	CodeGenerator(const std::string* path);
	virtual ~CodeGenerator();

	bool Generate(const Graph* graph, const Parallizer* parallizer);
};

#endif /* SRC_CODEGENERATOR_H_ */
