// src/sql/engine/expr/ob_expr_hello_repeat.cpp
#include "sql/engine/expr/ob_expr_hello_repeat.h"
#include "sql/engine/expr/ob_expr_util.h"
#include "sql/session/ob_sql_session_info.h"

using namespace oceanbase::common;
using namespace oceanbase::sql;

ObExprHelloRepeat::ObExprHelloRepeat(ObIAllocator &alloc)
    : ObFuncExprOperator(alloc, T_FUN_SYS_HELLO_REPEAT, N_HELLO_REPEAT, 1, NOT_ROW_DIMENSION)
{
}

int ObExprHelloRepeat::calc_result_type1(ObExprResType &type,
                                         ObExprResType &type1,
                                         ObExprTypeCtx &type_ctx) const
{
    UNUSED(type_ctx);
    type.set_varchar();
    type.set_length(OB_MAX_VARCHAR_LENGTH);
    type1.set_calc_type(ObIntType);
    return OB_SUCCESS;
}

int ObExprHelloRepeat::calc_result1(ObObj &result,
                                    const ObObj &obj1,
                                    ObExprCtx &expr_ctx) const
{
    int ret = OB_SUCCESS;
    if (obj1.is_null()) {
        result.set_null();
    } else {
        int64_t count = obj1.get_int();
        if (count <= 0) {
            result.set_varchar("");
        } else {
            ObString hello_str = "Hello, OceanBase!";
            ObString res_str;
            int64_t total_length = hello_str.length() * count;
            char *buf = static_cast<char*>(expr_ctx.calc_buf_->alloc(total_length));
            if (OB_ISNULL(buf)) {
                ret = OB_ALLOCATE_MEMORY_FAILED;
            } else {
                for (int64_t i = 0; i < count; ++i) {
                    MEMCPY(buf + i * hello_str.length(), hello_str.ptr(), hello_str.length());
                }
                res_str.assign_ptr(buf, static_cast<int32_t>(total_length));
                result.set_varchar(res_str);
            }
        }
    }
    return ret;
}

OB_SERIALIZE_MEMBER((ObExprHelloRepeat, ObFuncExprOperator));