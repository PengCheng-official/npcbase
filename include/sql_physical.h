#ifndef NPCBASE_SQL_PHYSICAL_H
#define NPCBASE_SQL_PHYSICAL_H

#include "sql_plan.h"
#include "index_manager.h"
#include "table_manager.h"
#include "data_dict.h"
#include <string>
#include <vector>

enum class PhysOpType { TableScan, IndexScan, Filter, Project };

struct PhysOp {
    PhysOpType type;
    std::string detail; // human-readable description
};

struct PhysicalPlan { std::vector<PhysOp> steps; };

// Decide physical operators from optimized logical plan
PhysicalPlan buildPhysicalPlan(const LogicalPlan& optPlan, DataDict& dict, IndexManager& idxMgr);

// Pretty print physical plan
std::string printPhysicalPlan(const PhysicalPlan& plan);

#endif

