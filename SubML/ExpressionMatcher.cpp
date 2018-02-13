#include "ExpressionMatcher.h"

namespace Ravel
{
    namespace SubML
    {
        ExpressionMatcher::ExpressionMatcher(OperatorMatcher * oper, ExpressionArgsMatcher * groupMatcher)
            : oper(oper), groupMatcher(groupMatcher)
        {
        }

        void ExpressionMatcher::BeginInternal()
        {
            Expression * expr = MatchArgument<0>();
            oper->Begin(match_captures, match_capture_count, expr->Oper());
            groupMatcher->Begin(oper->Next(), match_capture_count, expr->Args(), expr->ArgCount());
        }

        void ** ExpressionMatcher::NextInternal()
        {
            auto groupCaptures = groupMatcher->Next();
            while (!groupCaptures)
            {
                auto operCaptures = oper->Next();
                if (!operCaptures)
                {
                    Finish();
                    return nullptr;
                }

                Expression * expr = MatchArgument<0>();
                groupMatcher->Begin(operCaptures, match_capture_count, expr->Args(), expr->ArgCount());
                groupCaptures = groupMatcher->Next();
            }
            return groupCaptures;
        }
    }
}

