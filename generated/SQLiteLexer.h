
// Generated from D:/Pengcheng/University/Year3/DBMS/NpcBase/grammar/SQLiteLexer.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  SQLiteLexer : public antlr4::Lexer {
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

  explicit SQLiteLexer(antlr4::CharStream *input);

  ~SQLiteLexer() override;


  std::string getGrammarFileName() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const std::vector<std::string>& getChannelNames() const override;

  const std::vector<std::string>& getModeNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;

  const antlr4::atn::ATN& getATN() const override;

  // By default the static state used to implement the lexer is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:

  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

};

