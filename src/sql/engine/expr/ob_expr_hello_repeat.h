// src/sql/engine/expr/ob_expr_hello_repeat.h
#ifndef OCEANBASE_SQL_ENGINE_EXPR_OB_EXPR_HELLO_REPEAT_H_
#define OCEANBASE_SQL_ENGINE_EXPR_OB_EXPR_HELLO_REPEAT_H_

#include "sql/engine/expr/ob_expr_operator.h"

namespace oceanbase
{
namespace sql
{
class ObExprHelloRepeat : public ObFuncExprOperator
{
public:
    explicit ObExprHelloRepeat(common::ObIAllocator &alloc);
    virtual ~ObExprHelloRepeat() {}
    virtual int calc_result_type1(ObExprResType &type,
                                  ObExprResType &type1,
                                  ObExprTypeCtx &type_ctx) const override;
    virtual int calc_result1(common::ObObj &result,
                             const common::ObObj &obj1,
                             ObExprCtx &expr_ctx) const override;
private:
    DISALLOW_COPY_AND_ASSIGN(ObExprHelloRepeat);
};
} // namespace sql
} // namespace oceanbase

#endif // OCEANBASE_SQL_ENGINE_EXPR_OB_EXPR_HELLO_REPEAT_H_