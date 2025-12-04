#include "../include/sql_plan.h"
#include <sstream>

LogicalPlanResult buildLogicalPlan(const SqlSelect& select) {
    LogicalPlanResult res; res.ok = false;
    LogicalNode scan{LogicalOpType::Scan, select.table, {}, std::nullopt, {}};
    LogicalNode root = scan;
    // WHERE becomes Select above Scan
    if (select.where.has_value()) {
        LogicalNode sel{LogicalOpType::Select, "", {}, select.where, {root}};
        root = sel;
    }
    // Projection on top
    LogicalNode proj{LogicalOpType::Project, "", select.columns, std::nullopt, {root}};
    res.plan = LogicalPlan{proj};
    res.ok = true; return res;
}

OptimizeResult optimizeLogicalPlan(const LogicalPlan& plan, DataDict& dict) {
    OptimizeResult opt; opt.ok = true; opt.optimized = plan; // since only Scan/Select/Project, we keep order as Select->Scan under Project
    // Simple rule: if columns == {'*'} replace by actual columns from table metadata (projection normalization)
    auto& root = opt.optimized.root;
    if (root.type == LogicalOpType::Project && root.children.size() == 1) {
        auto& child = root.children[0];
        const LogicalNode* scanNode = nullptr;
        if (child.type == LogicalOpType::Select && !child.children.empty()) {
            if (child.children[0].type == LogicalOpType::Scan) scanNode = &child.children[0];
        } else if (child.type == LogicalOpType::Scan) {
            scanNode = &child;
        }
        if (scanNode) {
            if (root.columns.size() == 1 && root.columns[0] == "*") {
                TableInfo ti; RC rc = dict.findTable(scanNode->table.c_str(), ti);
                if (rc == RC_OK) {
                    root.columns.clear();
                    for (int i=0;i<ti.attrCount;i++) {
                        root.columns.push_back(ti.attrs[i].name);
                    }
                }
            }
        }
    }
    return opt;
}

static std::string opName(LogicalOpType t){
    switch(t){case LogicalOpType::Scan: return "Scan"; case LogicalOpType::Select: return "Select"; case LogicalOpType::Project: return "Project";}
    return "?";
}

static void printNode(const LogicalNode& n, int depth, std::ostringstream& out){
    for(int i=0;i<depth;i++) out << "  ";
    out << opName(n.type);
    if (n.type == LogicalOpType::Scan) {
        out << "(table=" << n.table << ")";
    }
    if (n.type == LogicalOpType::Select && n.predicate.has_value()) {
        out << "(pred=" << n.predicate->column << " " << n.predicate->op << " '" << n.predicate->literal << "')";
    }
    if (n.type == LogicalOpType::Project) {
        out << "(cols=";
        for(size_t i=0;i<n.columns.size();++i){ if(i) out << ", "; out << n.columns[i]; }
        out << ")";
    }
    out << "\n";
    for (const auto& c : n.children) printNode(c, depth+1, out);
}

std::string printLogicalPlan(const LogicalPlan& plan){
    std::ostringstream out; printNode(plan.root, 0, out); return out.str();
}

