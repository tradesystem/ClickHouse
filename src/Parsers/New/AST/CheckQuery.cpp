#include <Parsers/New/AST/CheckQuery.h>

#include <Interpreters/StorageID.h>
#include <Parsers/ASTCheckQuery.h>
#include <Parsers/ASTIdentifier.h>
#include <Parsers/New/AST/AlterTableQuery.h>
#include <Parsers/New/AST/Identifier.h>
#include <Parsers/New/ParseTreeVisitor.h>


namespace DB::AST
{

CheckQuery::CheckQuery(PtrTo<TableIdentifier> identifier, PtrTo<PartitionClause> clause) : Query{identifier, clause}
{
}

ASTPtr CheckQuery::convertToOld() const
{
    auto query = std::make_shared<ASTCheckQuery>();

    auto table = std::static_pointer_cast<ASTTableIdentifier>(get(NAME)->convertToOld());
    query->database = table->getDatabaseName();
    query->table = table->shortName();

    if (has(PARTITION)) query->partition = get(PARTITION)->convertToOld();

    return query;
}

}

namespace DB
{

using namespace AST;

antlrcpp::Any ParseTreeVisitor::visitCheckStmt(ClickHouseParser::CheckStmtContext *ctx)
{
    auto partition = ctx->partitionClause() ? visit(ctx->partitionClause()).as<PtrTo<PartitionClause>>() : nullptr;
    return std::make_shared<CheckQuery>(visit(ctx->tableIdentifier()), partition);
}

}
