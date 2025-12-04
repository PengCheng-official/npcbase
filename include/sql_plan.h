#ifndef NPCBASE_SQL_PLAN_H
#define NPCBASE_SQL_PLAN_H

#include "sql_ast.h"
#include "data_dict.h"
#include <string>
#include <vector>
#include <optional>

// Logical algebra operators
enum class LogicalOpType { Scan, Select, Project };

struct LogicalNode {
    LogicalOpType type;
    std::string table; // for Scan
    std::vector<std::string> columns; // for Project
    std::optional<SqlExpr> predicate; // for Select
    std::vector<LogicalNode> children; // unary/linear tree
};

struct LogicalPlan { LogicalNode root; };

// Build logical plan from parsed AST
struct LogicalPlanResult { bool ok{false}; std::string error; LogicalPlan plan; };
LogicalPlanResult buildLogicalPlan(const SqlSelect& select);

// Optimize logical plan with simple rules
struct OptimizeResult { bool ok{false}; std::string error; LogicalPlan optimized; };
OptimizeResult optimizeLogicalPlan(const LogicalPlan& plan, DataDict& dict);

// Pretty print logical plan
std::string printLogicalPlan(const LogicalPlan& plan);

#endif

