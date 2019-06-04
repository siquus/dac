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

#include "Graph.h"
#include "Module.h"

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
		float_,
	};

	Variable(const std::string* identifier, properties_t properties, Type type, size_t length = 1, const void* value = nullptr);

	bool GetDeclaration(std::string* decl) const;
	const std::string * GetIdentifier() const;
	bool GetElement(std::string* elem, uint32_t elemIndex) const;
	size_t Length() const;
	bool HasProperty(properties_t property) const;
	bool AddProperty(properties_t property);

private:
	properties_t properties_;
	Type type_;
	size_t length_;
	std::string identifier_;
	const void* value_;
};

class CodeGenerator {
	std::string path_;
	FILE *outfile_ = nullptr;
	FILE *outHeaderFile_ = nullptr;
	const Graph* graph_ = nullptr;

	bool GenerateConstants();
	bool GenerateStatics();
	bool GenerateIncludes();
	bool GenerateHeading(const std::string * heading);
	bool GenerateOutputFunctions();
	bool GenerateConstantDeclarations();
	bool GenerateStaticDeclarations();
	bool FetchVariables();

	std::map<Graph::NodeId_t, Variable> variables_;

public:
	CodeGenerator(const std::string* path);
	virtual ~CodeGenerator();

	bool Generate(const Graph* graph);
};

#endif /* SRC_CODEGENERATOR_H_ */
