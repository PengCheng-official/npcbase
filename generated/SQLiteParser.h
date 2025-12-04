
// Generated from D:/Pengcheng/University/Year3/DBMS/NpcBase/grammar/SQLiteParser.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  SQLiteParser : public antlr4::Parser {
public:
  enum {
    SCOL = 1, DOT = 2, OPEN_PAR = 3, CLOSE_PAR = 4, COMMA = 5, ASSIGN = 6, 
    STAR = 7, PLUS = 8, MINUS = 9, TILDE = 10, PIPE2 = 11, DIV = 12, MOD = 13, 
    LT2 = 14, GT2 = 15, AMP = 16, PIPE = 17, LT = 18, LT_EQ = 19, GT = 20, 
    GT_EQ = 21, EQ = 22, NOT_EQ1 = 23, NOT_EQ2 = 24, JPTR = 25, JPTR2 = 26, 
    ABORT_ = 27, ACTION_ = 28, ADD_ = 29, AFTER_ = 30, ALL_ = 31, ALTER_ = 32, 
    ANALYZE_ = 33, AND_ = 34, AS_ = 35, ASC_ = 36, ATTACH_ = 37, AUTOINCREMENT_ = 38, 
    BEFORE_ = 39, BEGIN_ = 40, BETWEEN_ = 41, BY_ = 42, CASCADE_ = 43, CASE_ = 44, 
    CAST_ = 45, CHECK_ = 46, COLLATE_ = 47, COLUMN_ = 48, COMMIT_ = 49, 
    CONFLICT_ = 50, CONSTRAINT_ = 51, CREATE_ = 52, CROSS_ = 53, CURRENT_DATE_ = 54, 
    CURRENT_TIME_ = 55, CURRENT_TIMESTAMP_ = 56, DATABASE_ = 57, DEFAULT_ = 58, 
    DEFERRABLE_ = 59, DEFERRED_ = 60, DELETE_ = 61, DESC_ = 62, DETACH_ = 63, 
    DISTINCT_ = 64, DROP_ = 65, EACH_ = 66, ELSE_ = 67, END_ = 68, ESCAPE_ = 69, 
    EXCEPT_ = 70, EXCLUSIVE_ = 71, EXISTS_ = 72, EXPLAIN_ = 73, FAIL_ = 74, 
    FOR_ = 75, FOREIGN_ = 76, FROM_ = 77, FULL_ = 78, GLOB_ = 79, GROUP_ = 80, 
    HAVING_ = 81, IF_ = 82, IGNORE_ = 83, IMMEDIATE_ = 84, IN_ = 85, INDEX_ = 86, 
    INDEXED_ = 87, INITIALLY_ = 88, INNER_ = 89, INSERT_ = 90, INSTEAD_ = 91, 
    INTERSECT_ = 92, INTO_ = 93, IS_ = 94, ISNULL_ = 95, JOIN_ = 96, KEY_ = 97, 
    LEFT_ = 98, LIKE_ = 99, LIMIT_ = 100, MATCH_ = 101, MATERIALIZED_ = 102, 
    NATURAL_ = 103, NO_ = 104, NOT_ = 105, NOTNULL_ = 106, NULL_ = 107, 
    OF_ = 108, OFFSET_ = 109, ON_ = 110, OR_ = 111, ORDER_ = 112, OUTER_ = 113, 
    PLAN_ = 114, PRAGMA_ = 115, PRIMARY_ = 116, QUERY_ = 117, RAISE_ = 118, 
    RECURSIVE_ = 119, REFERENCES_ = 120, REGEXP_ = 121, REINDEX_ = 122, 
    RELEASE_ = 123, RENAME_ = 124, REPLACE_ = 125, RESTRICT_ = 126, RETURNING_ = 127, 
    RIGHT_ = 128, ROLLBACK_ = 129, ROW_ = 130, ROWS_ = 131, ROWID_ = 132, 
    SAVEPOINT_ = 133, SELECT_ = 134, SET_ = 135, STRICT_ = 136, TABLE_ = 137, 
    TEMP_ = 138, TEMPORARY_ = 139, THEN_ = 140, TO_ = 141, TRANSACTION_ = 142, 
    TRIGGER_ = 143, UNION_ = 144, UNIQUE_ = 145, UPDATE_ = 146, USING_ = 147, 
    VACUUM_ = 148, VALUES_ = 149, VIEW_ = 150, VIRTUAL_ = 151, WHEN_ = 152, 
    WHERE_ = 153, WITH_ = 154, WITHOUT_ = 155, OVER_ = 156, PARTITION_ = 157, 
    RANGE_ = 158, PRECEDING_ = 159, UNBOUNDED_ = 160, CURRENT_ = 161, FOLLOWING_ = 162, 
    RANK_ = 163, GENERATED_ = 164, ALWAYS_ = 165, STORED_ = 166, TRUE_ = 167, 
    FALSE_ = 168, WINDOW_ = 169, NULLS_ = 170, FIRST_ = 171, LAST_ = 172, 
    FILTER_ = 173, GROUPS_ = 174, EXCLUDE_ = 175, TIES_ = 176, OTHERS_ = 177, 
    DO_ = 178, NOTHING_ = 179, IDENTIFIER = 180, NUMERIC_LITERAL = 181, 
    BIND_PARAMETER = 182, STRING_LITERAL = 183, BLOB_LITERAL = 184, SINGLE_LINE_COMMENT = 185, 
    MULTILINE_COMMENT = 186, SPACES = 187, UNEXPECTED_CHAR = 188
  };

  enum {
    RuleParse = 0, RuleSql_stmt_list = 1, RuleSql_stmt = 2, RuleAlter_table_stmt = 3, 
    RuleAnalyze_stmt = 4, RuleAttach_stmt = 5, RuleBegin_stmt = 6, RuleCommit_stmt = 7, 
    RuleRollback_stmt = 8, RuleSavepoint_stmt = 9, RuleRelease_stmt = 10, 
    RuleCreate_index_stmt = 11, RuleIndexed_column = 12, RuleCreate_table_stmt = 13, 
    RuleTable_options = 14, RuleColumn_def = 15, RuleType_name = 16, RuleColumn_constraint = 17, 
    RuleSigned_number = 18, RuleTable_constraint = 19, RuleForeign_key_clause = 20, 
    RuleConflict_clause = 21, RuleCreate_trigger_stmt = 22, RuleCreate_view_stmt = 23, 
    RuleCreate_virtual_table_stmt = 24, RuleWith_clause = 25, RuleCommon_table_expression = 26, 
    RuleCte_table_name = 27, RuleDelete_stmt = 28, RuleDelete_stmt_limited = 29, 
    RuleDetach_stmt = 30, RuleDrop_stmt = 31, RuleExpr = 32, RuleRaise_function = 33, 
    RuleLiteral_value = 34, RuleValue_row = 35, RuleValues_clause = 36, 
    RuleInsert_stmt = 37, RuleReturning_clause = 38, RuleUpsert_clause = 39, 
    RulePragma_stmt = 40, RulePragma_value = 41, RuleReindex_stmt = 42, 
    RuleSelect_stmt = 43, RuleJoin_clause = 44, RuleSelect_core = 45, RuleTable_or_subquery = 46, 
    RuleResult_column = 47, RuleJoin_operator = 48, RuleJoin_constraint = 49, 
    RuleCompound_operator = 50, RuleUpdate_stmt = 51, RuleColumn_name_list = 52, 
    RuleUpdate_stmt_limited = 53, RuleQualified_table_name = 54, RuleVacuum_stmt = 55, 
    RuleFilter_clause = 56, RuleWindow_defn = 57, RuleOver_clause = 58, 
    RuleFrame_spec = 59, RuleFrame_clause = 60, RuleOrder_clause = 61, RuleLimit_clause = 62, 
    RuleOrdering_term = 63, RuleAsc_desc = 64, RuleFrame_left = 65, RuleFrame_right = 66, 
    RuleFrame_single = 67, RuleError_message = 68, RuleFilename = 69, RuleModule_argument = 70, 
    RuleKeyword = 71, RuleName = 72, RuleFunction_name = 73, RuleSchema_name = 74, 
    RuleTable_name = 75, RuleTable_or_index_name = 76, RuleColumn_name = 77, 
    RuleColumn_alias = 78, RuleCollation_name = 79, RuleForeign_table = 80, 
    RuleIndex_name = 81, RuleTrigger_name = 82, RuleView_name = 83, RuleModule_name = 84, 
    RulePragma_name = 85, RuleSavepoint_name = 86, RuleTable_alias = 87, 
    RuleWindow_name = 88, RuleAlias = 89, RuleBase_window_name = 90, RuleTable_function_name = 91, 
    RuleFactored_select_stmt = 92, RuleSimple_select_stmt = 93, RuleCompound_select_stmt = 94, 
    RuleRecursive_cte = 95, RuleInitial_select = 96, RuleRecursive_select = 97, 
    RuleSimple_function_invocation = 98, RuleAggregate_function_invocation = 99, 
    RuleWindow_function_invocation = 100, RuleSimple_func = 101, RuleAggregate_func = 102, 
    RuleWindow_func = 103, RuleAny_name = 104
  };

  explicit SQLiteParser(antlr4::TokenStream *input);

  SQLiteParser(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~SQLiteParser() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  class ParseContext;
  class Sql_stmt_listContext;
  class Sql_stmtContext;
  class Alter_table_stmtContext;
  class Analyze_stmtContext;
  class Attach_stmtContext;
  class Begin_stmtContext;
  class Commit_stmtContext;
  class Rollback_stmtContext;
  class Savepoint_stmtContext;
  class Release_stmtContext;
  class Create_index_stmtContext;
  class Indexed_columnContext;
  class Create_table_stmtContext;
  class Table_optionsContext;
  class Column_defContext;
  class Type_nameContext;
  class Column_constraintContext;
  class Signed_numberContext;
  class Table_constraintContext;
  class Foreign_key_clauseContext;
  class Conflict_clauseContext;
  class Create_trigger_stmtContext;
  class Create_view_stmtContext;
  class Create_virtual_table_stmtContext;
  class With_clauseContext;
  class Common_table_expressionContext;
  class Cte_table_nameContext;
  class Delete_stmtContext;
  class Delete_stmt_limitedContext;
  class Detach_stmtContext;
  class Drop_stmtContext;
  class ExprContext;
  class Raise_functionContext;
  class Literal_valueContext;
  class Value_rowContext;
  class Values_clauseContext;
  class Insert_stmtContext;
  class Returning_clauseContext;
  class Upsert_clauseContext;
  class Pragma_stmtContext;
  class Pragma_valueContext;
  class Reindex_stmtContext;
  class Select_stmtContext;
  class Join_clauseContext;
  class Select_coreContext;
  class Table_or_subqueryContext;
  class Result_columnContext;
  class Join_operatorContext;
  class Join_constraintContext;
  class Compound_operatorContext;
  class Update_stmtContext;
  class Column_name_listContext;
  class Update_stmt_limitedContext;
  class Qualified_table_nameContext;
  class Vacuum_stmtContext;
  class Filter_clauseContext;
  class Window_defnContext;
  class Over_clauseContext;
  class Frame_specContext;
  class Frame_clauseContext;
  class Order_clauseContext;
  class Limit_clauseContext;
  class Ordering_termContext;
  class Asc_descContext;
  class Frame_leftContext;
  class Frame_rightContext;
  class Frame_singleContext;
  class Error_messageContext;
  class FilenameContext;
  class Module_argumentContext;
  class KeywordContext;
  class NameContext;
  class Function_nameContext;
  class Schema_nameContext;
  class Table_nameContext;
  class Table_or_index_nameContext;
  class Column_nameContext;
  class Column_aliasContext;
  class Collation_nameContext;
  class Foreign_tableContext;
  class Index_nameContext;
  class Trigger_nameContext;
  class View_nameContext;
  class Module_nameContext;
  class Pragma_nameContext;
  class Savepoint_nameContext;
  class Table_aliasContext;
  class Window_nameContext;
  class AliasContext;
  class Base_window_nameContext;
  class Table_function_nameContext;
  class Factored_select_stmtContext;
  class Simple_select_stmtContext;
  class Compound_select_stmtContext;
  class Recursive_cteContext;
  class Initial_selectContext;
  class Recursive_selectContext;
  class Simple_function_invocationContext;
  class Aggregate_function_invocationContext;
  class Window_function_invocationContext;
  class Simple_funcContext;
  class Aggregate_funcContext;
  class Window_funcContext;
  class Any_nameContext; 

  class  ParseContext : public antlr4::ParserRuleContext {
  public:
    ParseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Sql_stmt_listContext *sql_stmt_list();
    antlr4::tree::TerminalNode *EOF();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ParseContext* parse();

  class  Sql_stmt_listContext : public antlr4::ParserRuleContext {
  public:
    Sql_stmt_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Sql_stmtContext *> sql_stmt();
    Sql_stmtContext* sql_stmt(size_t i);
    std::vector<antlr4::tree::TerminalNode *> SCOL();
    antlr4::tree::TerminalNode* SCOL(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Sql_stmt_listContext* sql_stmt_list();

  class  Sql_stmtContext : public antlr4::ParserRuleContext {
  public:
    Sql_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Alter_table_stmtContext *alter_table_stmt();
    Analyze_stmtContext *analyze_stmt();
    Attach_stmtContext *attach_stmt();
    Begin_stmtContext *begin_stmt();
    Commit_stmtContext *commit_stmt();
    Create_index_stmtContext *create_index_stmt();
    Create_table_stmtContext *create_table_stmt();
    Create_trigger_stmtContext *create_trigger_stmt();
    Create_view_stmtContext *create_view_stmt();
    Create_virtual_table_stmtContext *create_virtual_table_stmt();
    Delete_stmtContext *delete_stmt();
    Delete_stmt_limitedContext *delete_stmt_limited();
    Detach_stmtContext *detach_stmt();
    Drop_stmtContext *drop_stmt();
    Insert_stmtContext *insert_stmt();
    Pragma_stmtContext *pragma_stmt();
    Reindex_stmtContext *reindex_stmt();
    Release_stmtContext *release_stmt();
    Rollback_stmtContext *rollback_stmt();
    Savepoint_stmtContext *savepoint_stmt();
    Select_stmtContext *select_stmt();
    Update_stmtContext *update_stmt();
    Update_stmt_limitedContext *update_stmt_limited();
    Vacuum_stmtContext *vacuum_stmt();
    antlr4::tree::TerminalNode *EXPLAIN_();
    antlr4::tree::TerminalNode *QUERY_();
    antlr4::tree::TerminalNode *PLAN_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Sql_stmtContext* sql_stmt();

  class  Alter_table_stmtContext : public antlr4::ParserRuleContext {
  public:
    SQLiteParser::Table_nameContext *new_table_name = nullptr;
    SQLiteParser::Column_nameContext *old_column_name = nullptr;
    SQLiteParser::Column_nameContext *new_column_name = nullptr;
    Alter_table_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ALTER_();
    antlr4::tree::TerminalNode *TABLE_();
    std::vector<Table_nameContext *> table_name();
    Table_nameContext* table_name(size_t i);
    antlr4::tree::TerminalNode *RENAME_();
    antlr4::tree::TerminalNode *ADD_();
    Column_defContext *column_def();
    antlr4::tree::TerminalNode *DROP_();
    std::vector<Column_nameContext *> column_name();
    Column_nameContext* column_name(size_t i);
    Schema_nameContext *schema_name();
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *TO_();
    antlr4::tree::TerminalNode *COLUMN_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Alter_table_stmtContext* alter_table_stmt();

  class  Analyze_stmtContext : public antlr4::ParserRuleContext {
  public:
    Analyze_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ANALYZE_();
    Schema_nameContext *schema_name();
    Table_or_index_nameContext *table_or_index_name();
    antlr4::tree::TerminalNode *DOT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Analyze_stmtContext* analyze_stmt();

  class  Attach_stmtContext : public antlr4::ParserRuleContext {
  public:
    Attach_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ATTACH_();
    ExprContext *expr();
    antlr4::tree::TerminalNode *AS_();
    Schema_nameContext *schema_name();
    antlr4::tree::TerminalNode *DATABASE_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Attach_stmtContext* attach_stmt();

  class  Begin_stmtContext : public antlr4::ParserRuleContext {
  public:
    Begin_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *BEGIN_();
    antlr4::tree::TerminalNode *TRANSACTION_();
    antlr4::tree::TerminalNode *DEFERRED_();
    antlr4::tree::TerminalNode *IMMEDIATE_();
    antlr4::tree::TerminalNode *EXCLUSIVE_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Begin_stmtContext* begin_stmt();

  class  Commit_stmtContext : public antlr4::ParserRuleContext {
  public:
    Commit_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COMMIT_();
    antlr4::tree::TerminalNode *END_();
    antlr4::tree::TerminalNode *TRANSACTION_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Commit_stmtContext* commit_stmt();

  class  Rollback_stmtContext : public antlr4::ParserRuleContext {
  public:
    Rollback_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ROLLBACK_();
    antlr4::tree::TerminalNode *TRANSACTION_();
    antlr4::tree::TerminalNode *TO_();
    Savepoint_nameContext *savepoint_name();
    antlr4::tree::TerminalNode *SAVEPOINT_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Rollback_stmtContext* rollback_stmt();

  class  Savepoint_stmtContext : public antlr4::ParserRuleContext {
  public:
    Savepoint_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SAVEPOINT_();
    Savepoint_nameContext *savepoint_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Savepoint_stmtContext* savepoint_stmt();

  class  Release_stmtContext : public antlr4::ParserRuleContext {
  public:
    Release_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *RELEASE_();
    Savepoint_nameContext *savepoint_name();
    antlr4::tree::TerminalNode *SAVEPOINT_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Release_stmtContext* release_stmt();

  class  Create_index_stmtContext : public antlr4::ParserRuleContext {
  public:
    Create_index_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CREATE_();
    antlr4::tree::TerminalNode *INDEX_();
    Index_nameContext *index_name();
    antlr4::tree::TerminalNode *ON_();
    Table_nameContext *table_name();
    antlr4::tree::TerminalNode *OPEN_PAR();
    std::vector<Indexed_columnContext *> indexed_column();
    Indexed_columnContext* indexed_column(size_t i);
    antlr4::tree::TerminalNode *CLOSE_PAR();
    antlr4::tree::TerminalNode *UNIQUE_();
    antlr4::tree::TerminalNode *IF_();
    antlr4::tree::TerminalNode *NOT_();
    antlr4::tree::TerminalNode *EXISTS_();
    Schema_nameContext *schema_name();
    antlr4::tree::TerminalNode *DOT();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    antlr4::tree::TerminalNode *WHERE_();
    ExprContext *expr();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Create_index_stmtContext* create_index_stmt();

  class  Indexed_columnContext : public antlr4::ParserRuleContext {
  public:
    Indexed_columnContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Column_nameContext *column_name();
    ExprContext *expr();
    antlr4::tree::TerminalNode *COLLATE_();
    Collation_nameContext *collation_name();
    Asc_descContext *asc_desc();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Indexed_columnContext* indexed_column();

  class  Create_table_stmtContext : public antlr4::ParserRuleContext {
  public:
    Create_table_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CREATE_();
    antlr4::tree::TerminalNode *TABLE_();
    Table_nameContext *table_name();
    antlr4::tree::TerminalNode *OPEN_PAR();
    std::vector<Column_defContext *> column_def();
    Column_defContext* column_def(size_t i);
    antlr4::tree::TerminalNode *CLOSE_PAR();
    antlr4::tree::TerminalNode *AS_();
    Select_stmtContext *select_stmt();
    antlr4::tree::TerminalNode *IF_();
    antlr4::tree::TerminalNode *NOT_();
    antlr4::tree::TerminalNode *EXISTS_();
    Schema_nameContext *schema_name();
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *TEMP_();
    antlr4::tree::TerminalNode *TEMPORARY_();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    std::vector<Table_constraintContext *> table_constraint();
    Table_constraintContext* table_constraint(size_t i);
    Table_optionsContext *table_options();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Create_table_stmtContext* create_table_stmt();

  class  Table_optionsContext : public antlr4::ParserRuleContext {
  public:
    Table_optionsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> WITHOUT_();
    antlr4::tree::TerminalNode* WITHOUT_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> ROWID_();
    antlr4::tree::TerminalNode* ROWID_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> STRICT_();
    antlr4::tree::TerminalNode* STRICT_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Table_optionsContext* table_options();

  class  Column_defContext : public antlr4::ParserRuleContext {
  public:
    Column_defContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Column_nameContext *column_name();
    Type_nameContext *type_name();
    std::vector<Column_constraintContext *> column_constraint();
    Column_constraintContext* column_constraint(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Column_defContext* column_def();

  class  Type_nameContext : public antlr4::ParserRuleContext {
  public:
    Type_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<NameContext *> name();
    NameContext* name(size_t i);
    antlr4::tree::TerminalNode *OPEN_PAR();
    std::vector<Signed_numberContext *> signed_number();
    Signed_numberContext* signed_number(size_t i);
    antlr4::tree::TerminalNode *CLOSE_PAR();
    antlr4::tree::TerminalNode *COMMA();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Type_nameContext* type_name();

  class  Column_constraintContext : public antlr4::ParserRuleContext {
  public:
    Column_constraintContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CHECK_();
    antlr4::tree::TerminalNode *OPEN_PAR();
    ExprContext *expr();
    antlr4::tree::TerminalNode *CLOSE_PAR();
    antlr4::tree::TerminalNode *DEFAULT_();
    antlr4::tree::TerminalNode *COLLATE_();
    Collation_nameContext *collation_name();
    Foreign_key_clauseContext *foreign_key_clause();
    antlr4::tree::TerminalNode *AS_();
    antlr4::tree::TerminalNode *CONSTRAINT_();
    NameContext *name();
    antlr4::tree::TerminalNode *PRIMARY_();
    antlr4::tree::TerminalNode *KEY_();
    antlr4::tree::TerminalNode *NULL_();
    antlr4::tree::TerminalNode *UNIQUE_();
    Signed_numberContext *signed_number();
    Literal_valueContext *literal_value();
    Conflict_clauseContext *conflict_clause();
    antlr4::tree::TerminalNode *GENERATED_();
    antlr4::tree::TerminalNode *ALWAYS_();
    antlr4::tree::TerminalNode *STORED_();
    antlr4::tree::TerminalNode *VIRTUAL_();
    Asc_descContext *asc_desc();
    antlr4::tree::TerminalNode *AUTOINCREMENT_();
    antlr4::tree::TerminalNode *NOT_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Column_constraintContext* column_constraint();

  class  Signed_numberContext : public antlr4::ParserRuleContext {
  public:
    Signed_numberContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NUMERIC_LITERAL();
    antlr4::tree::TerminalNode *PLUS();
    antlr4::tree::TerminalNode *MINUS();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Signed_numberContext* signed_number();

  class  Table_constraintContext : public antlr4::ParserRuleContext {
  public:
    Table_constraintContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *OPEN_PAR();
    std::vector<Indexed_columnContext *> indexed_column();
    Indexed_columnContext* indexed_column(size_t i);
    antlr4::tree::TerminalNode *CLOSE_PAR();
    antlr4::tree::TerminalNode *CHECK_();
    ExprContext *expr();
    antlr4::tree::TerminalNode *FOREIGN_();
    antlr4::tree::TerminalNode *KEY_();
    std::vector<Column_nameContext *> column_name();
    Column_nameContext* column_name(size_t i);
    Foreign_key_clauseContext *foreign_key_clause();
    antlr4::tree::TerminalNode *CONSTRAINT_();
    NameContext *name();
    antlr4::tree::TerminalNode *PRIMARY_();
    antlr4::tree::TerminalNode *UNIQUE_();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    Conflict_clauseContext *conflict_clause();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Table_constraintContext* table_constraint();

  class  Foreign_key_clauseContext : public antlr4::ParserRuleContext {
  public:
    Foreign_key_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *REFERENCES_();
    Foreign_tableContext *foreign_table();
    antlr4::tree::TerminalNode *OPEN_PAR();
    std::vector<Column_nameContext *> column_name();
    Column_nameContext* column_name(size_t i);
    antlr4::tree::TerminalNode *CLOSE_PAR();
    std::vector<antlr4::tree::TerminalNode *> ON_();
    antlr4::tree::TerminalNode* ON_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> MATCH_();
    antlr4::tree::TerminalNode* MATCH_(size_t i);
    std::vector<NameContext *> name();
    NameContext* name(size_t i);
    antlr4::tree::TerminalNode *DEFERRABLE_();
    std::vector<antlr4::tree::TerminalNode *> DELETE_();
    antlr4::tree::TerminalNode* DELETE_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> UPDATE_();
    antlr4::tree::TerminalNode* UPDATE_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> SET_();
    antlr4::tree::TerminalNode* SET_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> CASCADE_();
    antlr4::tree::TerminalNode* CASCADE_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> RESTRICT_();
    antlr4::tree::TerminalNode* RESTRICT_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> NO_();
    antlr4::tree::TerminalNode* NO_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> ACTION_();
    antlr4::tree::TerminalNode* ACTION_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    std::vector<antlr4::tree::TerminalNode *> NULL_();
    antlr4::tree::TerminalNode* NULL_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> DEFAULT_();
    antlr4::tree::TerminalNode* DEFAULT_(size_t i);
    antlr4::tree::TerminalNode *NOT_();
    antlr4::tree::TerminalNode *INITIALLY_();
    antlr4::tree::TerminalNode *DEFERRED_();
    antlr4::tree::TerminalNode *IMMEDIATE_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Foreign_key_clauseContext* foreign_key_clause();

  class  Conflict_clauseContext : public antlr4::ParserRuleContext {
  public:
    Conflict_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ON_();
    antlr4::tree::TerminalNode *CONFLICT_();
    antlr4::tree::TerminalNode *ROLLBACK_();
    antlr4::tree::TerminalNode *ABORT_();
    antlr4::tree::TerminalNode *FAIL_();
    antlr4::tree::TerminalNode *IGNORE_();
    antlr4::tree::TerminalNode *REPLACE_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Conflict_clauseContext* conflict_clause();

  class  Create_trigger_stmtContext : public antlr4::ParserRuleContext {
  public:
    Create_trigger_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CREATE_();
    antlr4::tree::TerminalNode *TRIGGER_();
    Trigger_nameContext *trigger_name();
    antlr4::tree::TerminalNode *ON_();
    Table_nameContext *table_name();
    antlr4::tree::TerminalNode *BEGIN_();
    antlr4::tree::TerminalNode *END_();
    antlr4::tree::TerminalNode *DELETE_();
    antlr4::tree::TerminalNode *INSERT_();
    antlr4::tree::TerminalNode *UPDATE_();
    antlr4::tree::TerminalNode *IF_();
    antlr4::tree::TerminalNode *NOT_();
    antlr4::tree::TerminalNode *EXISTS_();
    Schema_nameContext *schema_name();
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *BEFORE_();
    antlr4::tree::TerminalNode *AFTER_();
    antlr4::tree::TerminalNode *INSTEAD_();
    std::vector<antlr4::tree::TerminalNode *> OF_();
    antlr4::tree::TerminalNode* OF_(size_t i);
    antlr4::tree::TerminalNode *FOR_();
    antlr4::tree::TerminalNode *EACH_();
    antlr4::tree::TerminalNode *ROW_();
    antlr4::tree::TerminalNode *WHEN_();
    ExprContext *expr();
    std::vector<antlr4::tree::TerminalNode *> SCOL();
    antlr4::tree::TerminalNode* SCOL(size_t i);
    antlr4::tree::TerminalNode *TEMP_();
    antlr4::tree::TerminalNode *TEMPORARY_();
    std::vector<Column_nameContext *> column_name();
    Column_nameContext* column_name(size_t i);
    std::vector<Update_stmtContext *> update_stmt();
    Update_stmtContext* update_stmt(size_t i);
    std::vector<Insert_stmtContext *> insert_stmt();
    Insert_stmtContext* insert_stmt(size_t i);
    std::vector<Delete_stmtContext *> delete_stmt();
    Delete_stmtContext* delete_stmt(size_t i);
    std::vector<Select_stmtContext *> select_stmt();
    Select_stmtContext* select_stmt(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Create_trigger_stmtContext* create_trigger_stmt();

  class  Create_view_stmtContext : public antlr4::ParserRuleContext {
  public:
    Create_view_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CREATE_();
    antlr4::tree::TerminalNode *VIEW_();
    View_nameContext *view_name();
    antlr4::tree::TerminalNode *AS_();
    Select_stmtContext *select_stmt();
    antlr4::tree::TerminalNode *IF_();
    antlr4::tree::TerminalNode *NOT_();
    antlr4::tree::TerminalNode *EXISTS_();
    Schema_nameContext *schema_name();
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *OPEN_PAR();
    std::vector<Column_nameContext *> column_name();
    Column_nameContext* column_name(size_t i);
    antlr4::tree::TerminalNode *CLOSE_PAR();
    antlr4::tree::TerminalNode *TEMP_();
    antlr4::tree::TerminalNode *TEMPORARY_();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Create_view_stmtContext* create_view_stmt();

  class  Create_virtual_table_stmtContext : public antlr4::ParserRuleContext {
  public:
    Create_virtual_table_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CREATE_();
    antlr4::tree::TerminalNode *VIRTUAL_();
    antlr4::tree::TerminalNode *TABLE_();
    Table_nameContext *table_name();
    antlr4::tree::TerminalNode *USING_();
    Module_nameContext *module_name();
    antlr4::tree::TerminalNode *IF_();
    antlr4::tree::TerminalNode *NOT_();
    antlr4::tree::TerminalNode *EXISTS_();
    Schema_nameContext *schema_name();
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *OPEN_PAR();
    std::vector<Module_argumentContext *> module_argument();
    Module_argumentContext* module_argument(size_t i);
    antlr4::tree::TerminalNode *CLOSE_PAR();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Create_virtual_table_stmtContext* create_virtual_table_stmt();

  class  With_clauseContext : public antlr4::ParserRuleContext {
  public:
    With_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *WITH_();
    std::vector<Common_table_expressionContext *> common_table_expression();
    Common_table_expressionContext* common_table_expression(size_t i);
    antlr4::tree::TerminalNode *RECURSIVE_();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  With_clauseContext* with_clause();

  class  Common_table_expressionContext : public antlr4::ParserRuleContext {
  public:
    Common_table_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Cte_table_nameContext *cte_table_name();
    antlr4::tree::TerminalNode *AS_();
    antlr4::tree::TerminalNode *OPEN_PAR();
    Select_stmtContext *select_stmt();
    antlr4::tree::TerminalNode *CLOSE_PAR();
    antlr4::tree::TerminalNode *MATERIALIZED_();
    antlr4::tree::TerminalNode *NOT_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Common_table_expressionContext* common_table_expression();

  class  Cte_table_nameContext : public antlr4::ParserRuleContext {
  public:
    Cte_table_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Table_nameContext *table_name();
    antlr4::tree::TerminalNode *OPEN_PAR();
    std::vector<Column_nameContext *> column_name();
    Column_nameContext* column_name(size_t i);
    antlr4::tree::TerminalNode *CLOSE_PAR();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Cte_table_nameContext* cte_table_name();

  class  Delete_stmtContext : public antlr4::ParserRuleContext {
  public:
    Delete_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DELETE_();
    antlr4::tree::TerminalNode *FROM_();
    Qualified_table_nameContext *qualified_table_name();
    With_clauseContext *with_clause();
    antlr4::tree::TerminalNode *WHERE_();
    ExprContext *expr();
    Returning_clauseContext *returning_clause();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Delete_stmtContext* delete_stmt();

  class  Delete_stmt_limitedContext : public antlr4::ParserRuleContext {
  public:
    Delete_stmt_limitedContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DELETE_();
    antlr4::tree::TerminalNode *FROM_();
    Qualified_table_nameContext *qualified_table_name();
    With_clauseContext *with_clause();
    antlr4::tree::TerminalNode *WHERE_();
    ExprContext *expr();
    Returning_clauseContext *returning_clause();
    Order_clauseContext *order_clause();
    Limit_clauseContext *limit_clause();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Delete_stmt_limitedContext* delete_stmt_limited();

  class  Detach_stmtContext : public antlr4::ParserRuleContext {
  public:
    Detach_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DETACH_();
    Schema_nameContext *schema_name();
    antlr4::tree::TerminalNode *DATABASE_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Detach_stmtContext* detach_stmt();

  class  Drop_stmtContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *object = nullptr;
    Drop_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DROP_();
    Any_nameContext *any_name();
    antlr4::tree::TerminalNode *INDEX_();
    antlr4::tree::TerminalNode *TABLE_();
    antlr4::tree::TerminalNode *TRIGGER_();
    antlr4::tree::TerminalNode *VIEW_();
    antlr4::tree::TerminalNode *IF_();
    antlr4::tree::TerminalNode *EXISTS_();
    Schema_nameContext *schema_name();
    antlr4::tree::TerminalNode *DOT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Drop_stmtContext* drop_stmt();

  class  ExprContext : public antlr4::ParserRuleContext {
  public:
    ExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Literal_valueContext *literal_value();
    antlr4::tree::TerminalNode *BIND_PARAMETER();
    Column_nameContext *column_name();
    Table_nameContext *table_name();
    std::vector<antlr4::tree::TerminalNode *> DOT();
    antlr4::tree::TerminalNode* DOT(size_t i);
    Schema_nameContext *schema_name();
    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);
    antlr4::tree::TerminalNode *MINUS();
    antlr4::tree::TerminalNode *PLUS();
    antlr4::tree::TerminalNode *TILDE();
    antlr4::tree::TerminalNode *NOT_();
    Function_nameContext *function_name();
    antlr4::tree::TerminalNode *OPEN_PAR();
    antlr4::tree::TerminalNode *CLOSE_PAR();
    antlr4::tree::TerminalNode *STAR();
    Filter_clauseContext *filter_clause();
    Over_clauseContext *over_clause();
    antlr4::tree::TerminalNode *DISTINCT_();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    Order_clauseContext *order_clause();
    antlr4::tree::TerminalNode *CAST_();
    antlr4::tree::TerminalNode *AS_();
    Type_nameContext *type_name();
    Select_stmtContext *select_stmt();
    antlr4::tree::TerminalNode *EXISTS_();
    antlr4::tree::TerminalNode *CASE_();
    antlr4::tree::TerminalNode *END_();
    std::vector<antlr4::tree::TerminalNode *> WHEN_();
    antlr4::tree::TerminalNode* WHEN_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> THEN_();
    antlr4::tree::TerminalNode* THEN_(size_t i);
    antlr4::tree::TerminalNode *ELSE_();
    Raise_functionContext *raise_function();
    antlr4::tree::TerminalNode *PIPE2();
    antlr4::tree::TerminalNode *JPTR();
    antlr4::tree::TerminalNode *JPTR2();
    antlr4::tree::TerminalNode *DIV();
    antlr4::tree::TerminalNode *MOD();
    antlr4::tree::TerminalNode *LT2();
    antlr4::tree::TerminalNode *GT2();
    antlr4::tree::TerminalNode *AMP();
    antlr4::tree::TerminalNode *PIPE();
    antlr4::tree::TerminalNode *LT();
    antlr4::tree::TerminalNode *LT_EQ();
    antlr4::tree::TerminalNode *GT();
    antlr4::tree::TerminalNode *GT_EQ();
    antlr4::tree::TerminalNode *ASSIGN();
    antlr4::tree::TerminalNode *EQ();
    antlr4::tree::TerminalNode *NOT_EQ1();
    antlr4::tree::TerminalNode *NOT_EQ2();
    antlr4::tree::TerminalNode *IS_();
    antlr4::tree::TerminalNode *FROM_();
    antlr4::tree::TerminalNode *BETWEEN_();
    antlr4::tree::TerminalNode *AND_();
    antlr4::tree::TerminalNode *OR_();
    antlr4::tree::TerminalNode *COLLATE_();
    Collation_nameContext *collation_name();
    antlr4::tree::TerminalNode *IN_();
    Table_function_nameContext *table_function_name();
    antlr4::tree::TerminalNode *GLOB_();
    antlr4::tree::TerminalNode *REGEXP_();
    antlr4::tree::TerminalNode *MATCH_();
    antlr4::tree::TerminalNode *LIKE_();
    antlr4::tree::TerminalNode *ESCAPE_();
    antlr4::tree::TerminalNode *ISNULL_();
    antlr4::tree::TerminalNode *NOTNULL_();
    antlr4::tree::TerminalNode *NULL_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExprContext* expr();
  ExprContext* expr(int precedence);
  class  Raise_functionContext : public antlr4::ParserRuleContext {
  public:
    Raise_functionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *RAISE_();
    antlr4::tree::TerminalNode *OPEN_PAR();
    antlr4::tree::TerminalNode *CLOSE_PAR();
    antlr4::tree::TerminalNode *IGNORE_();
    antlr4::tree::TerminalNode *COMMA();
    Error_messageContext *error_message();
    antlr4::tree::TerminalNode *ROLLBACK_();
    antlr4::tree::TerminalNode *ABORT_();
    antlr4::tree::TerminalNode *FAIL_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Raise_functionContext* raise_function();

  class  Literal_valueContext : public antlr4::ParserRuleContext {
  public:
    Literal_valueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NUMERIC_LITERAL();
    antlr4::tree::TerminalNode *STRING_LITERAL();
    antlr4::tree::TerminalNode *BLOB_LITERAL();
    antlr4::tree::TerminalNode *NULL_();
    antlr4::tree::TerminalNode *TRUE_();
    antlr4::tree::TerminalNode *FALSE_();
    antlr4::tree::TerminalNode *CURRENT_TIME_();
    antlr4::tree::TerminalNode *CURRENT_DATE_();
    antlr4::tree::TerminalNode *CURRENT_TIMESTAMP_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Literal_valueContext* literal_value();

  class  Value_rowContext : public antlr4::ParserRuleContext {
  public:
    Value_rowContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *OPEN_PAR();
    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);
    antlr4::tree::TerminalNode *CLOSE_PAR();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Value_rowContext* value_row();

  class  Values_clauseContext : public antlr4::ParserRuleContext {
  public:
    Values_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *VALUES_();
    std::vector<Value_rowContext *> value_row();
    Value_rowContext* value_row(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Values_clauseContext* values_clause();

  class  Insert_stmtContext : public antlr4::ParserRuleContext {
  public:
    Insert_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *INTO_();
    Table_nameContext *table_name();
    antlr4::tree::TerminalNode *INSERT_();
    antlr4::tree::TerminalNode *REPLACE_();
    antlr4::tree::TerminalNode *OR_();
    antlr4::tree::TerminalNode *DEFAULT_();
    antlr4::tree::TerminalNode *VALUES_();
    With_clauseContext *with_clause();
    antlr4::tree::TerminalNode *ROLLBACK_();
    antlr4::tree::TerminalNode *ABORT_();
    antlr4::tree::TerminalNode *FAIL_();
    antlr4::tree::TerminalNode *IGNORE_();
    Schema_nameContext *schema_name();
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *AS_();
    Table_aliasContext *table_alias();
    antlr4::tree::TerminalNode *OPEN_PAR();
    std::vector<Column_nameContext *> column_name();
    Column_nameContext* column_name(size_t i);
    antlr4::tree::TerminalNode *CLOSE_PAR();
    Returning_clauseContext *returning_clause();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    Values_clauseContext *values_clause();
    Select_stmtContext *select_stmt();
    std::vector<Upsert_clauseContext *> upsert_clause();
    Upsert_clauseContext* upsert_clause(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Insert_stmtContext* insert_stmt();

  class  Returning_clauseContext : public antlr4::ParserRuleContext {
  public:
    Returning_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *RETURNING_();
    std::vector<antlr4::tree::TerminalNode *> STAR();
    antlr4::tree::TerminalNode* STAR(size_t i);
    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    std::vector<Column_aliasContext *> column_alias();
    Column_aliasContext* column_alias(size_t i);
    std::vector<antlr4::tree::TerminalNode *> AS_();
    antlr4::tree::TerminalNode* AS_(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Returning_clauseContext* returning_clause();

  class  Upsert_clauseContext : public antlr4::ParserRuleContext {
  public:
    Upsert_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ON_();
    antlr4::tree::TerminalNode *CONFLICT_();
    antlr4::tree::TerminalNode *DO_();
    antlr4::tree::TerminalNode *NOTHING_();
    antlr4::tree::TerminalNode *UPDATE_();
    antlr4::tree::TerminalNode *SET_();
    antlr4::tree::TerminalNode *OPEN_PAR();
    std::vector<Indexed_columnContext *> indexed_column();
    Indexed_columnContext* indexed_column(size_t i);
    antlr4::tree::TerminalNode *CLOSE_PAR();
    std::vector<antlr4::tree::TerminalNode *> ASSIGN();
    antlr4::tree::TerminalNode* ASSIGN(size_t i);
    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    std::vector<antlr4::tree::TerminalNode *> WHERE_();
    antlr4::tree::TerminalNode* WHERE_(size_t i);
    std::vector<Column_nameContext *> column_name();
    Column_nameContext* column_name(size_t i);
    std::vector<Column_name_listContext *> column_name_list();
    Column_name_listContext* column_name_list(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Upsert_clauseContext* upsert_clause();

  class  Pragma_stmtContext : public antlr4::ParserRuleContext {
  public:
    Pragma_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *PRAGMA_();
    Pragma_nameContext *pragma_name();
    Schema_nameContext *schema_name();
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *ASSIGN();
    Pragma_valueContext *pragma_value();
    antlr4::tree::TerminalNode *OPEN_PAR();
    antlr4::tree::TerminalNode *CLOSE_PAR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Pragma_stmtContext* pragma_stmt();

  class  Pragma_valueContext : public antlr4::ParserRuleContext {
  public:
    Pragma_valueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Signed_numberContext *signed_number();
    NameContext *name();
    antlr4::tree::TerminalNode *STRING_LITERAL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Pragma_valueContext* pragma_value();

  class  Reindex_stmtContext : public antlr4::ParserRuleContext {
  public:
    Reindex_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *REINDEX_();
    Collation_nameContext *collation_name();
    Table_nameContext *table_name();
    Index_nameContext *index_name();
    Schema_nameContext *schema_name();
    antlr4::tree::TerminalNode *DOT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Reindex_stmtContext* reindex_stmt();

  class  Select_stmtContext : public antlr4::ParserRuleContext {
  public:
    Select_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Select_coreContext *> select_core();
    Select_coreContext* select_core(size_t i);
    With_clauseContext *with_clause();
    std::vector<Compound_operatorContext *> compound_operator();
    Compound_operatorContext* compound_operator(size_t i);
    Order_clauseContext *order_clause();
    Limit_clauseContext *limit_clause();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Select_stmtContext* select_stmt();

  class  Join_clauseContext : public antlr4::ParserRuleContext {
  public:
    Join_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Table_or_subqueryContext *> table_or_subquery();
    Table_or_subqueryContext* table_or_subquery(size_t i);
    std::vector<Join_operatorContext *> join_operator();
    Join_operatorContext* join_operator(size_t i);
    std::vector<Join_constraintContext *> join_constraint();
    Join_constraintContext* join_constraint(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Join_clauseContext* join_clause();

  class  Select_coreContext : public antlr4::ParserRuleContext {
  public:
    SQLiteParser::ExprContext *where_expr = nullptr;
    SQLiteParser::ExprContext *exprContext = nullptr;
    std::vector<ExprContext *> group_by_expr;
    SQLiteParser::ExprContext *having_expr = nullptr;
    Select_coreContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SELECT_();
    std::vector<Result_columnContext *> result_column();
    Result_columnContext* result_column(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    antlr4::tree::TerminalNode *FROM_();
    antlr4::tree::TerminalNode *WHERE_();
    antlr4::tree::TerminalNode *GROUP_();
    antlr4::tree::TerminalNode *BY_();
    antlr4::tree::TerminalNode *WINDOW_();
    std::vector<Window_nameContext *> window_name();
    Window_nameContext* window_name(size_t i);
    std::vector<antlr4::tree::TerminalNode *> AS_();
    antlr4::tree::TerminalNode* AS_(size_t i);
    std::vector<Window_defnContext *> window_defn();
    Window_defnContext* window_defn(size_t i);
    antlr4::tree::TerminalNode *DISTINCT_();
    antlr4::tree::TerminalNode *ALL_();
    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);
    std::vector<Table_or_subqueryContext *> table_or_subquery();
    Table_or_subqueryContext* table_or_subquery(size_t i);
    Join_clauseContext *join_clause();
    antlr4::tree::TerminalNode *HAVING_();
    Values_clauseContext *values_clause();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Select_coreContext* select_core();

  class  Table_or_subqueryContext : public antlr4::ParserRuleContext {
  public:
    Table_or_subqueryContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Table_nameContext *table_name();
    Schema_nameContext *schema_name();
    antlr4::tree::TerminalNode *DOT();
    Table_aliasContext *table_alias();
    antlr4::tree::TerminalNode *INDEXED_();
    antlr4::tree::TerminalNode *BY_();
    Index_nameContext *index_name();
    antlr4::tree::TerminalNode *NOT_();
    antlr4::tree::TerminalNode *AS_();
    Table_function_nameContext *table_function_name();
    antlr4::tree::TerminalNode *OPEN_PAR();
    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);
    antlr4::tree::TerminalNode *CLOSE_PAR();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    std::vector<Table_or_subqueryContext *> table_or_subquery();
    Table_or_subqueryContext* table_or_subquery(size_t i);
    Join_clauseContext *join_clause();
    Select_stmtContext *select_stmt();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Table_or_subqueryContext* table_or_subquery();

  class  Result_columnContext : public antlr4::ParserRuleContext {
  public:
    Result_columnContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *STAR();
    Table_nameContext *table_name();
    antlr4::tree::TerminalNode *DOT();
    ExprContext *expr();
    Column_aliasContext *column_alias();
    antlr4::tree::TerminalNode *AS_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Result_columnContext* result_column();

  class  Join_operatorContext : public antlr4::ParserRuleContext {
  public:
    Join_operatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COMMA();
    antlr4::tree::TerminalNode *JOIN_();
    antlr4::tree::TerminalNode *NATURAL_();
    antlr4::tree::TerminalNode *INNER_();
    antlr4::tree::TerminalNode *CROSS_();
    antlr4::tree::TerminalNode *LEFT_();
    antlr4::tree::TerminalNode *RIGHT_();
    antlr4::tree::TerminalNode *FULL_();
    antlr4::tree::TerminalNode *OUTER_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Join_operatorContext* join_operator();

  class  Join_constraintContext : public antlr4::ParserRuleContext {
  public:
    Join_constraintContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ON_();
    ExprContext *expr();
    antlr4::tree::TerminalNode *USING_();
    antlr4::tree::TerminalNode *OPEN_PAR();
    std::vector<Column_nameContext *> column_name();
    Column_nameContext* column_name(size_t i);
    antlr4::tree::TerminalNode *CLOSE_PAR();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Join_constraintContext* join_constraint();

  class  Compound_operatorContext : public antlr4::ParserRuleContext {
  public:
    Compound_operatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *UNION_();
    antlr4::tree::TerminalNode *ALL_();
    antlr4::tree::TerminalNode *INTERSECT_();
    antlr4::tree::TerminalNode *EXCEPT_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Compound_operatorContext* compound_operator();

  class  Update_stmtContext : public antlr4::ParserRuleContext {
  public:
    Update_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *UPDATE_();
    Qualified_table_nameContext *qualified_table_name();
    antlr4::tree::TerminalNode *SET_();
    std::vector<antlr4::tree::TerminalNode *> ASSIGN();
    antlr4::tree::TerminalNode* ASSIGN(size_t i);
    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);
    std::vector<Column_nameContext *> column_name();
    Column_nameContext* column_name(size_t i);
    std::vector<Column_name_listContext *> column_name_list();
    Column_name_listContext* column_name_list(size_t i);
    With_clauseContext *with_clause();
    antlr4::tree::TerminalNode *OR_();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    antlr4::tree::TerminalNode *FROM_();
    antlr4::tree::TerminalNode *WHERE_();
    Returning_clauseContext *returning_clause();
    antlr4::tree::TerminalNode *ROLLBACK_();
    antlr4::tree::TerminalNode *ABORT_();
    antlr4::tree::TerminalNode *REPLACE_();
    antlr4::tree::TerminalNode *FAIL_();
    antlr4::tree::TerminalNode *IGNORE_();
    std::vector<Table_or_subqueryContext *> table_or_subquery();
    Table_or_subqueryContext* table_or_subquery(size_t i);
    Join_clauseContext *join_clause();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Update_stmtContext* update_stmt();

  class  Column_name_listContext : public antlr4::ParserRuleContext {
  public:
    Column_name_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *OPEN_PAR();
    std::vector<Column_nameContext *> column_name();
    Column_nameContext* column_name(size_t i);
    antlr4::tree::TerminalNode *CLOSE_PAR();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Column_name_listContext* column_name_list();

  class  Update_stmt_limitedContext : public antlr4::ParserRuleContext {
  public:
    Update_stmt_limitedContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *UPDATE_();
    Qualified_table_nameContext *qualified_table_name();
    antlr4::tree::TerminalNode *SET_();
    std::vector<antlr4::tree::TerminalNode *> ASSIGN();
    antlr4::tree::TerminalNode* ASSIGN(size_t i);
    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);
    std::vector<Column_nameContext *> column_name();
    Column_nameContext* column_name(size_t i);
    std::vector<Column_name_listContext *> column_name_list();
    Column_name_listContext* column_name_list(size_t i);
    With_clauseContext *with_clause();
    antlr4::tree::TerminalNode *OR_();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    antlr4::tree::TerminalNode *FROM_();
    antlr4::tree::TerminalNode *WHERE_();
    Returning_clauseContext *returning_clause();
    Order_clauseContext *order_clause();
    Limit_clauseContext *limit_clause();
    antlr4::tree::TerminalNode *ROLLBACK_();
    antlr4::tree::TerminalNode *ABORT_();
    antlr4::tree::TerminalNode *REPLACE_();
    antlr4::tree::TerminalNode *FAIL_();
    antlr4::tree::TerminalNode *IGNORE_();
    std::vector<Table_or_subqueryContext *> table_or_subquery();
    Table_or_subqueryContext* table_or_subquery(size_t i);
    Join_clauseContext *join_clause();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Update_stmt_limitedContext* update_stmt_limited();

  class  Qualified_table_nameContext : public antlr4::ParserRuleContext {
  public:
    Qualified_table_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Table_nameContext *table_name();
    Schema_nameContext *schema_name();
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *AS_();
    AliasContext *alias();
    antlr4::tree::TerminalNode *INDEXED_();
    antlr4::tree::TerminalNode *BY_();
    Index_nameContext *index_name();
    antlr4::tree::TerminalNode *NOT_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Qualified_table_nameContext* qualified_table_name();

  class  Vacuum_stmtContext : public antlr4::ParserRuleContext {
  public:
    Vacuum_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *VACUUM_();
    Schema_nameContext *schema_name();
    antlr4::tree::TerminalNode *INTO_();
    FilenameContext *filename();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Vacuum_stmtContext* vacuum_stmt();

  class  Filter_clauseContext : public antlr4::ParserRuleContext {
  public:
    Filter_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *FILTER_();
    antlr4::tree::TerminalNode *OPEN_PAR();
    antlr4::tree::TerminalNode *WHERE_();
    ExprContext *expr();
    antlr4::tree::TerminalNode *CLOSE_PAR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Filter_clauseContext* filter_clause();

  class  Window_defnContext : public antlr4::ParserRuleContext {
  public:
    Window_defnContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *OPEN_PAR();
    antlr4::tree::TerminalNode *CLOSE_PAR();
    Base_window_nameContext *base_window_name();
    antlr4::tree::TerminalNode *PARTITION_();
    antlr4::tree::TerminalNode *BY_();
    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);
    Order_clauseContext *order_clause();
    Frame_specContext *frame_spec();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Window_defnContext* window_defn();

  class  Over_clauseContext : public antlr4::ParserRuleContext {
  public:
    Over_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *OVER_();
    Window_nameContext *window_name();
    antlr4::tree::TerminalNode *OPEN_PAR();
    antlr4::tree::TerminalNode *CLOSE_PAR();
    Base_window_nameContext *base_window_name();
    antlr4::tree::TerminalNode *PARTITION_();
    antlr4::tree::TerminalNode *BY_();
    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);
    Order_clauseContext *order_clause();
    Frame_specContext *frame_spec();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Over_clauseContext* over_clause();

  class  Frame_specContext : public antlr4::ParserRuleContext {
  public:
    Frame_specContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Frame_clauseContext *frame_clause();
    antlr4::tree::TerminalNode *EXCLUDE_();
    antlr4::tree::TerminalNode *NO_();
    antlr4::tree::TerminalNode *OTHERS_();
    antlr4::tree::TerminalNode *CURRENT_();
    antlr4::tree::TerminalNode *ROW_();
    antlr4::tree::TerminalNode *GROUP_();
    antlr4::tree::TerminalNode *TIES_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Frame_specContext* frame_spec();

  class  Frame_clauseContext : public antlr4::ParserRuleContext {
  public:
    Frame_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *RANGE_();
    antlr4::tree::TerminalNode *ROWS_();
    antlr4::tree::TerminalNode *GROUPS_();
    Frame_singleContext *frame_single();
    antlr4::tree::TerminalNode *BETWEEN_();
    Frame_leftContext *frame_left();
    antlr4::tree::TerminalNode *AND_();
    Frame_rightContext *frame_right();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Frame_clauseContext* frame_clause();

  class  Order_clauseContext : public antlr4::ParserRuleContext {
  public:
    Order_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ORDER_();
    antlr4::tree::TerminalNode *BY_();
    std::vector<Ordering_termContext *> ordering_term();
    Ordering_termContext* ordering_term(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Order_clauseContext* order_clause();

  class  Limit_clauseContext : public antlr4::ParserRuleContext {
  public:
    Limit_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LIMIT_();
    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);
    antlr4::tree::TerminalNode *OFFSET_();
    antlr4::tree::TerminalNode *COMMA();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Limit_clauseContext* limit_clause();

  class  Ordering_termContext : public antlr4::ParserRuleContext {
  public:
    Ordering_termContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExprContext *expr();
    antlr4::tree::TerminalNode *COLLATE_();
    Collation_nameContext *collation_name();
    Asc_descContext *asc_desc();
    antlr4::tree::TerminalNode *NULLS_();
    antlr4::tree::TerminalNode *FIRST_();
    antlr4::tree::TerminalNode *LAST_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Ordering_termContext* ordering_term();

  class  Asc_descContext : public antlr4::ParserRuleContext {
  public:
    Asc_descContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ASC_();
    antlr4::tree::TerminalNode *DESC_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Asc_descContext* asc_desc();

  class  Frame_leftContext : public antlr4::ParserRuleContext {
  public:
    Frame_leftContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExprContext *expr();
    antlr4::tree::TerminalNode *PRECEDING_();
    antlr4::tree::TerminalNode *FOLLOWING_();
    antlr4::tree::TerminalNode *CURRENT_();
    antlr4::tree::TerminalNode *ROW_();
    antlr4::tree::TerminalNode *UNBOUNDED_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Frame_leftContext* frame_left();

  class  Frame_rightContext : public antlr4::ParserRuleContext {
  public:
    Frame_rightContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExprContext *expr();
    antlr4::tree::TerminalNode *PRECEDING_();
    antlr4::tree::TerminalNode *FOLLOWING_();
    antlr4::tree::TerminalNode *CURRENT_();
    antlr4::tree::TerminalNode *ROW_();
    antlr4::tree::TerminalNode *UNBOUNDED_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Frame_rightContext* frame_right();

  class  Frame_singleContext : public antlr4::ParserRuleContext {
  public:
    Frame_singleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExprContext *expr();
    antlr4::tree::TerminalNode *PRECEDING_();
    antlr4::tree::TerminalNode *UNBOUNDED_();
    antlr4::tree::TerminalNode *CURRENT_();
    antlr4::tree::TerminalNode *ROW_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Frame_singleContext* frame_single();

  class  Error_messageContext : public antlr4::ParserRuleContext {
  public:
    Error_messageContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExprContext *expr();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Error_messageContext* error_message();

  class  FilenameContext : public antlr4::ParserRuleContext {
  public:
    FilenameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExprContext *expr();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FilenameContext* filename();

  class  Module_argumentContext : public antlr4::ParserRuleContext {
  public:
    Module_argumentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> OPEN_PAR();
    antlr4::tree::TerminalNode* OPEN_PAR(size_t i);
    std::vector<antlr4::tree::TerminalNode *> CLOSE_PAR();
    antlr4::tree::TerminalNode* CLOSE_PAR(size_t i);
    std::vector<Module_argumentContext *> module_argument();
    Module_argumentContext* module_argument(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Module_argumentContext* module_argument();

  class  KeywordContext : public antlr4::ParserRuleContext {
  public:
    KeywordContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ABORT_();
    antlr4::tree::TerminalNode *ACTION_();
    antlr4::tree::TerminalNode *ADD_();
    antlr4::tree::TerminalNode *AFTER_();
    antlr4::tree::TerminalNode *ALL_();
    antlr4::tree::TerminalNode *ALTER_();
    antlr4::tree::TerminalNode *ANALYZE_();
    antlr4::tree::TerminalNode *AND_();
    antlr4::tree::TerminalNode *AS_();
    antlr4::tree::TerminalNode *ASC_();
    antlr4::tree::TerminalNode *ATTACH_();
    antlr4::tree::TerminalNode *AUTOINCREMENT_();
    antlr4::tree::TerminalNode *BEFORE_();
    antlr4::tree::TerminalNode *BEGIN_();
    antlr4::tree::TerminalNode *BETWEEN_();
    antlr4::tree::TerminalNode *BY_();
    antlr4::tree::TerminalNode *CASCADE_();
    antlr4::tree::TerminalNode *CASE_();
    antlr4::tree::TerminalNode *CAST_();
    antlr4::tree::TerminalNode *CHECK_();
    antlr4::tree::TerminalNode *COLLATE_();
    antlr4::tree::TerminalNode *COLUMN_();
    antlr4::tree::TerminalNode *COMMIT_();
    antlr4::tree::TerminalNode *CONFLICT_();
    antlr4::tree::TerminalNode *CONSTRAINT_();
    antlr4::tree::TerminalNode *CREATE_();
    antlr4::tree::TerminalNode *CROSS_();
    antlr4::tree::TerminalNode *CURRENT_DATE_();
    antlr4::tree::TerminalNode *CURRENT_TIME_();
    antlr4::tree::TerminalNode *CURRENT_TIMESTAMP_();
    antlr4::tree::TerminalNode *DATABASE_();
    antlr4::tree::TerminalNode *DEFAULT_();
    antlr4::tree::TerminalNode *DEFERRABLE_();
    antlr4::tree::TerminalNode *DEFERRED_();
    antlr4::tree::TerminalNode *DELETE_();
    antlr4::tree::TerminalNode *DESC_();
    antlr4::tree::TerminalNode *DETACH_();
    antlr4::tree::TerminalNode *DISTINCT_();
    antlr4::tree::TerminalNode *DROP_();
    antlr4::tree::TerminalNode *EACH_();
    antlr4::tree::TerminalNode *ELSE_();
    antlr4::tree::TerminalNode *END_();
    antlr4::tree::TerminalNode *ESCAPE_();
    antlr4::tree::TerminalNode *EXCEPT_();
    antlr4::tree::TerminalNode *EXCLUSIVE_();
    antlr4::tree::TerminalNode *EXISTS_();
    antlr4::tree::TerminalNode *EXPLAIN_();
    antlr4::tree::TerminalNode *FAIL_();
    antlr4::tree::TerminalNode *FOR_();
    antlr4::tree::TerminalNode *FOREIGN_();
    antlr4::tree::TerminalNode *FROM_();
    antlr4::tree::TerminalNode *FULL_();
    antlr4::tree::TerminalNode *GLOB_();
    antlr4::tree::TerminalNode *GROUP_();
    antlr4::tree::TerminalNode *HAVING_();
    antlr4::tree::TerminalNode *IF_();
    antlr4::tree::TerminalNode *IGNORE_();
    antlr4::tree::TerminalNode *IMMEDIATE_();
    antlr4::tree::TerminalNode *IN_();
    antlr4::tree::TerminalNode *INDEX_();
    antlr4::tree::TerminalNode *INDEXED_();
    antlr4::tree::TerminalNode *INITIALLY_();
    antlr4::tree::TerminalNode *INNER_();
    antlr4::tree::TerminalNode *INSERT_();
    antlr4::tree::TerminalNode *INSTEAD_();
    antlr4::tree::TerminalNode *INTERSECT_();
    antlr4::tree::TerminalNode *INTO_();
    antlr4::tree::TerminalNode *IS_();
    antlr4::tree::TerminalNode *ISNULL_();
    antlr4::tree::TerminalNode *JOIN_();
    antlr4::tree::TerminalNode *KEY_();
    antlr4::tree::TerminalNode *LEFT_();
    antlr4::tree::TerminalNode *LIKE_();
    antlr4::tree::TerminalNode *LIMIT_();
    antlr4::tree::TerminalNode *MATCH_();
    antlr4::tree::TerminalNode *MATERIALIZED_();
    antlr4::tree::TerminalNode *NATURAL_();
    antlr4::tree::TerminalNode *NO_();
    antlr4::tree::TerminalNode *NOT_();
    antlr4::tree::TerminalNode *NOTNULL_();
    antlr4::tree::TerminalNode *NULL_();
    antlr4::tree::TerminalNode *OF_();
    antlr4::tree::TerminalNode *OFFSET_();
    antlr4::tree::TerminalNode *ON_();
    antlr4::tree::TerminalNode *OR_();
    antlr4::tree::TerminalNode *ORDER_();
    antlr4::tree::TerminalNode *OUTER_();
    antlr4::tree::TerminalNode *PLAN_();
    antlr4::tree::TerminalNode *PRAGMA_();
    antlr4::tree::TerminalNode *PRIMARY_();
    antlr4::tree::TerminalNode *QUERY_();
    antlr4::tree::TerminalNode *RAISE_();
    antlr4::tree::TerminalNode *RECURSIVE_();
    antlr4::tree::TerminalNode *REFERENCES_();
    antlr4::tree::TerminalNode *REGEXP_();
    antlr4::tree::TerminalNode *REINDEX_();
    antlr4::tree::TerminalNode *RELEASE_();
    antlr4::tree::TerminalNode *RENAME_();
    antlr4::tree::TerminalNode *REPLACE_();
    antlr4::tree::TerminalNode *RESTRICT_();
    antlr4::tree::TerminalNode *RETURNING_();
    antlr4::tree::TerminalNode *RIGHT_();
    antlr4::tree::TerminalNode *ROLLBACK_();
    antlr4::tree::TerminalNode *ROW_();
    antlr4::tree::TerminalNode *ROWS_();
    antlr4::tree::TerminalNode *ROWID_();
    antlr4::tree::TerminalNode *SAVEPOINT_();
    antlr4::tree::TerminalNode *SELECT_();
    antlr4::tree::TerminalNode *SET_();
    antlr4::tree::TerminalNode *STRICT_();
    antlr4::tree::TerminalNode *TABLE_();
    antlr4::tree::TerminalNode *TEMP_();
    antlr4::tree::TerminalNode *TEMPORARY_();
    antlr4::tree::TerminalNode *THEN_();
    antlr4::tree::TerminalNode *TO_();
    antlr4::tree::TerminalNode *TRANSACTION_();
    antlr4::tree::TerminalNode *TRIGGER_();
    antlr4::tree::TerminalNode *UNION_();
    antlr4::tree::TerminalNode *UNIQUE_();
    antlr4::tree::TerminalNode *UPDATE_();
    antlr4::tree::TerminalNode *USING_();
    antlr4::tree::TerminalNode *VACUUM_();
    antlr4::tree::TerminalNode *VALUES_();
    antlr4::tree::TerminalNode *VIEW_();
    antlr4::tree::TerminalNode *VIRTUAL_();
    antlr4::tree::TerminalNode *WHEN_();
    antlr4::tree::TerminalNode *WHERE_();
    antlr4::tree::TerminalNode *WITH_();
    antlr4::tree::TerminalNode *WITHOUT_();
    antlr4::tree::TerminalNode *OVER_();
    antlr4::tree::TerminalNode *PARTITION_();
    antlr4::tree::TerminalNode *RANGE_();
    antlr4::tree::TerminalNode *PRECEDING_();
    antlr4::tree::TerminalNode *UNBOUNDED_();
    antlr4::tree::TerminalNode *CURRENT_();
    antlr4::tree::TerminalNode *FOLLOWING_();
    antlr4::tree::TerminalNode *RANK_();
    antlr4::tree::TerminalNode *GENERATED_();
    antlr4::tree::TerminalNode *ALWAYS_();
    antlr4::tree::TerminalNode *STORED_();
    antlr4::tree::TerminalNode *TRUE_();
    antlr4::tree::TerminalNode *FALSE_();
    antlr4::tree::TerminalNode *WINDOW_();
    antlr4::tree::TerminalNode *NULLS_();
    antlr4::tree::TerminalNode *FIRST_();
    antlr4::tree::TerminalNode *LAST_();
    antlr4::tree::TerminalNode *FILTER_();
    antlr4::tree::TerminalNode *GROUPS_();
    antlr4::tree::TerminalNode *EXCLUDE_();
    antlr4::tree::TerminalNode *TIES_();
    antlr4::tree::TerminalNode *OTHERS_();
    antlr4::tree::TerminalNode *DO_();
    antlr4::tree::TerminalNode *NOTHING_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  KeywordContext* keyword();

  class  NameContext : public antlr4::ParserRuleContext {
  public:
    NameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  NameContext* name();

  class  Function_nameContext : public antlr4::ParserRuleContext {
  public:
    Function_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Function_nameContext* function_name();

  class  Schema_nameContext : public antlr4::ParserRuleContext {
  public:
    Schema_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Schema_nameContext* schema_name();

  class  Table_nameContext : public antlr4::ParserRuleContext {
  public:
    Table_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Table_nameContext* table_name();

  class  Table_or_index_nameContext : public antlr4::ParserRuleContext {
  public:
    Table_or_index_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Table_or_index_nameContext* table_or_index_name();

  class  Column_nameContext : public antlr4::ParserRuleContext {
  public:
    Column_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Column_nameContext* column_name();

  class  Column_aliasContext : public antlr4::ParserRuleContext {
  public:
    Column_aliasContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Column_aliasContext* column_alias();

  class  Collation_nameContext : public antlr4::ParserRuleContext {
  public:
    Collation_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Collation_nameContext* collation_name();

  class  Foreign_tableContext : public antlr4::ParserRuleContext {
  public:
    Foreign_tableContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Foreign_tableContext* foreign_table();

  class  Index_nameContext : public antlr4::ParserRuleContext {
  public:
    Index_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Index_nameContext* index_name();

  class  Trigger_nameContext : public antlr4::ParserRuleContext {
  public:
    Trigger_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Trigger_nameContext* trigger_name();

  class  View_nameContext : public antlr4::ParserRuleContext {
  public:
    View_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  View_nameContext* view_name();

  class  Module_nameContext : public antlr4::ParserRuleContext {
  public:
    Module_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Module_nameContext* module_name();

  class  Pragma_nameContext : public antlr4::ParserRuleContext {
  public:
    Pragma_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Pragma_nameContext* pragma_name();

  class  Savepoint_nameContext : public antlr4::ParserRuleContext {
  public:
    Savepoint_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Savepoint_nameContext* savepoint_name();

  class  Table_aliasContext : public antlr4::ParserRuleContext {
  public:
    Table_aliasContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Table_aliasContext* table_alias();

  class  Window_nameContext : public antlr4::ParserRuleContext {
  public:
    Window_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Window_nameContext* window_name();

  class  AliasContext : public antlr4::ParserRuleContext {
  public:
    AliasContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AliasContext* alias();

  class  Base_window_nameContext : public antlr4::ParserRuleContext {
  public:
    Base_window_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Base_window_nameContext* base_window_name();

  class  Table_function_nameContext : public antlr4::ParserRuleContext {
  public:
    Table_function_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Table_function_nameContext* table_function_name();

  class  Factored_select_stmtContext : public antlr4::ParserRuleContext {
  public:
    Factored_select_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Select_stmtContext *select_stmt();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Factored_select_stmtContext* factored_select_stmt();

  class  Simple_select_stmtContext : public antlr4::ParserRuleContext {
  public:
    Simple_select_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Select_coreContext *select_core();
    With_clauseContext *with_clause();
    Order_clauseContext *order_clause();
    Limit_clauseContext *limit_clause();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Simple_select_stmtContext* simple_select_stmt();

  class  Compound_select_stmtContext : public antlr4::ParserRuleContext {
  public:
    Compound_select_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Select_coreContext *> select_core();
    Select_coreContext* select_core(size_t i);
    With_clauseContext *with_clause();
    Order_clauseContext *order_clause();
    Limit_clauseContext *limit_clause();
    std::vector<antlr4::tree::TerminalNode *> UNION_();
    antlr4::tree::TerminalNode* UNION_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> INTERSECT_();
    antlr4::tree::TerminalNode* INTERSECT_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> EXCEPT_();
    antlr4::tree::TerminalNode* EXCEPT_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> ALL_();
    antlr4::tree::TerminalNode* ALL_(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Compound_select_stmtContext* compound_select_stmt();

  class  Recursive_cteContext : public antlr4::ParserRuleContext {
  public:
    Recursive_cteContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Cte_table_nameContext *cte_table_name();
    antlr4::tree::TerminalNode *AS_();
    antlr4::tree::TerminalNode *OPEN_PAR();
    Initial_selectContext *initial_select();
    antlr4::tree::TerminalNode *UNION_();
    Recursive_selectContext *recursive_select();
    antlr4::tree::TerminalNode *CLOSE_PAR();
    antlr4::tree::TerminalNode *ALL_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Recursive_cteContext* recursive_cte();

  class  Initial_selectContext : public antlr4::ParserRuleContext {
  public:
    Initial_selectContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Select_stmtContext *select_stmt();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Initial_selectContext* initial_select();

  class  Recursive_selectContext : public antlr4::ParserRuleContext {
  public:
    Recursive_selectContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Select_stmtContext *select_stmt();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Recursive_selectContext* recursive_select();

  class  Simple_function_invocationContext : public antlr4::ParserRuleContext {
  public:
    Simple_function_invocationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Simple_funcContext *simple_func();
    antlr4::tree::TerminalNode *OPEN_PAR();
    antlr4::tree::TerminalNode *CLOSE_PAR();
    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);
    antlr4::tree::TerminalNode *STAR();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Simple_function_invocationContext* simple_function_invocation();

  class  Aggregate_function_invocationContext : public antlr4::ParserRuleContext {
  public:
    Aggregate_function_invocationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Aggregate_funcContext *aggregate_func();
    antlr4::tree::TerminalNode *OPEN_PAR();
    antlr4::tree::TerminalNode *CLOSE_PAR();
    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);
    antlr4::tree::TerminalNode *STAR();
    Filter_clauseContext *filter_clause();
    antlr4::tree::TerminalNode *DISTINCT_();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    Order_clauseContext *order_clause();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Aggregate_function_invocationContext* aggregate_function_invocation();

  class  Window_function_invocationContext : public antlr4::ParserRuleContext {
  public:
    Window_function_invocationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Window_funcContext *window_func();
    antlr4::tree::TerminalNode *OPEN_PAR();
    antlr4::tree::TerminalNode *CLOSE_PAR();
    antlr4::tree::TerminalNode *OVER_();
    Window_defnContext *window_defn();
    Window_nameContext *window_name();
    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);
    antlr4::tree::TerminalNode *STAR();
    Filter_clauseContext *filter_clause();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Window_function_invocationContext* window_function_invocation();

  class  Simple_funcContext : public antlr4::ParserRuleContext {
  public:
    Simple_funcContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Simple_funcContext* simple_func();

  class  Aggregate_funcContext : public antlr4::ParserRuleContext {
  public:
    Aggregate_funcContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Aggregate_funcContext* aggregate_func();

  class  Window_funcContext : public antlr4::ParserRuleContext {
  public:
    Window_funcContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Window_funcContext* window_func();

  class  Any_nameContext : public antlr4::ParserRuleContext {
  public:
    Any_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();
    KeywordContext *keyword();
    antlr4::tree::TerminalNode *STRING_LITERAL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Any_nameContext* any_name();


  bool sempred(antlr4::RuleContext *_localctx, size_t ruleIndex, size_t predicateIndex) override;

  bool exprSempred(ExprContext *_localctx, size_t predicateIndex);

  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

