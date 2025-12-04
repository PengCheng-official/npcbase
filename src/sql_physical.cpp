#include "../include/sql_physical.h"
#include <sstream>

static std::string join(const std::vector<std::string>& v){ std::ostringstream o; for(size_t i=0;i<v.size();++i){ if(i) o<<", "; o<<v[i]; } return o.str(); }

PhysicalPlan buildPhysicalPlan(const LogicalPlan& optPlan, DataDict& dict, IndexManager& idxMgr) {
    PhysicalPlan pp;
    // Expect Project -> (Select) -> Scan
    const LogicalNode& proj = optPlan.root;
    const LogicalNode* sel = nullptr; const LogicalNode* scan = nullptr;
    if (!proj.children.empty()) {
        const LogicalNode& c = proj.children[0];
        if (c.type == LogicalOpType::Select) { sel = &c; if (!c.children.empty() && c.children[0].type == LogicalOpType::Scan) scan = &c.children[0]; }
        else if (c.type == LogicalOpType::Scan) { scan = &c; }
    }
    if (!scan) { pp.steps.push_back({PhysOpType::TableScan, "Invalid plan structure"}); return pp; }

    bool useIndex = false; std::string indexName;
    if (sel && sel->predicate.has_value()) {
        // check if there's an index on the predicate column via public APIs
        TableInfo ti; RC rcT = dict.findTable(scan->table.c_str(), ti);
        if (rcT == RC_OK) {
            std::vector<IndexInfo> idxs; RC rcL = dict.listIndexesForTable(ti.tableId, idxs);
            if (rcL == RC_OK) {
                for (const auto& ii : idxs) {
                    if (ii.columnName == sel->predicate->column) { useIndex = true; indexName = ii.indexName; break; }
                }
            }
        }
    }

    if (useIndex) {
        pp.steps.push_back({PhysOpType::IndexScan, std::string("IndexScan on ") + scan->table + " using index " + indexName + (sel? (std::string(", key='") + sel->predicate->literal + "'") : "")});
    } else {
        pp.steps.push_back({PhysOpType::TableScan, std::string("TableScan on ") + scan->table});
        if (sel && sel->predicate.has_value()) {
            pp.steps.push_back({PhysOpType::Filter, std::string("Filter where ") + sel->predicate->column + " " + sel->predicate->op + " '" + sel->predicate->literal + "'"});
        }
    }

    // Projection
    pp.steps.push_back({PhysOpType::Project, std::string("Project columns ") + join(proj.columns)});
    return pp;
}

std::string printPhysicalPlan(const PhysicalPlan& plan){
    std::ostringstream out;
    for(size_t i=0;i<plan.steps.size();++i){ out << (i+1) << ". " << plan.steps[i].detail << "\n"; }
    return out.str();
}
