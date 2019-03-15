/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
#pragma once

#include "ErrorInstance.h"
#include "InternalFunction.h"
#include "JSObject.h"
#include "ThrowScope.h"
#include <stdint.h>


namespace JSC {

class ExecState;
class VM;
class JSGlobalObject;
class JSObject;
class SourceCode;
class Structure;

enum class ErrorType : uint8_t {
    Error,
    EvalError,
    RangeError,
    ReferenceError,
    SyntaxError,
    TypeError,
    URIError,
};

// ExecState wrappers.
JSObject* createError(ExecState*, const String&, ErrorInstance::SourceAppender);
JSObject* createEvalError(ExecState*, const String&, ErrorInstance::SourceAppender);
JSObject* createRangeError(ExecState*, const String&, ErrorInstance::SourceAppender);
JSObject* createReferenceError(ExecState*, const String&, ErrorInstance::SourceAppender);
JSObject* createSyntaxError(ExecState*, const String&, ErrorInstance::SourceAppender);
JSObject* createTypeError(ExecState*, const String&, ErrorInstance::SourceAppender, RuntimeType);
JSObject* createNotEnoughArgumentsError(ExecState*, ErrorInstance::SourceAppender);
JSObject* createURIError(ExecState*, const String&, ErrorInstance::SourceAppender);


JS_EXPORT_PRIVATE JSObject* createError(ExecState*, const String&);
JS_EXPORT_PRIVATE JSObject* createEvalError(ExecState*, const String&);
JS_EXPORT_PRIVATE JSObject* createRangeError(ExecState*, const String&);
JS_EXPORT_PRIVATE JSObject* createReferenceError(ExecState*, const String&);
JS_EXPORT_PRIVATE JSObject* createSyntaxError(ExecState*, const String&);
JS_EXPORT_PRIVATE JSObject* createTypeError(ExecState*);
JS_EXPORT_PRIVATE JSObject* createTypeError(ExecState*, const String&);
JS_EXPORT_PRIVATE JSObject* createNotEnoughArgumentsError(ExecState*);
JS_EXPORT_PRIVATE JSObject* createURIError(ExecState*, const String&);
JS_EXPORT_PRIVATE JSObject* createOutOfMemoryError(ExecState*);

JS_EXPORT_PRIVATE JSObject* createError(ExecState*, ErrorType, const String&);


bool addErrorInfoAndGetBytecodeOffset(ExecState*, VM&, JSObject*, bool, CallFrame*&, unsigned* = nullptr);

JS_EXPORT_PRIVATE void addErrorInfo(ExecState*, JSObject*, bool); 
JSObject* addErrorInfo(ExecState*, JSObject* error, int line, const SourceCode&);

// Methods to throw Errors.

// Convenience wrappers, create an throw an exception with a default message.
JS_EXPORT_PRIVATE JSObject* throwConstructorCannotBeCalledAsFunctionTypeError(ExecState*, ThrowScope&, const char* constructorName);
JS_EXPORT_PRIVATE JSObject* throwTypeError(ExecState*, ThrowScope&);
JS_EXPORT_PRIVATE JSObject* throwTypeError(ExecState*, ThrowScope&, ASCIILiteral errorMessage);
JS_EXPORT_PRIVATE JSObject* throwTypeError(ExecState*, ThrowScope&, const String& errorMessage);
JS_EXPORT_PRIVATE JSObject* throwSyntaxError(ExecState*, ThrowScope&);
JS_EXPORT_PRIVATE JSObject* throwSyntaxError(ExecState*, ThrowScope&, const String& errorMessage);
inline JSObject* throwRangeError(ExecState* state, ThrowScope& scope, const String& errorMessage) { return throwException(state, scope, createRangeError(state, errorMessage)); }

// Convenience wrappers, wrap result as an EncodedJSValue.
inline void throwVMError(ExecState* exec, ThrowScope& scope, Exception* exception) { throwException(exec, scope, exception); }
inline EncodedJSValue throwVMError(ExecState* exec, ThrowScope& scope, JSValue error) { return JSValue::encode(throwException(exec, scope, error)); }
inline EncodedJSValue throwVMError(ExecState* exec, ThrowScope& scope, const char* errorMessage) { return JSValue::encode(throwException(exec, scope, createError(exec, ASCIILiteral(errorMessage)))); }
inline EncodedJSValue throwVMTypeError(ExecState* exec, ThrowScope& scope) { return JSValue::encode(throwTypeError(exec, scope)); }
inline EncodedJSValue throwVMTypeError(ExecState* exec, ThrowScope& scope, ASCIILiteral errorMessage) { return JSValue::encode(throwTypeError(exec, scope, errorMessage)); }
inline EncodedJSValue throwVMTypeError(ExecState* exec, ThrowScope& scope, const String& errorMessage) { return JSValue::encode(throwTypeError(exec, scope, errorMessage)); }
inline EncodedJSValue throwVMRangeError(ExecState* state, ThrowScope& scope, const String& errorMessage) { return JSValue::encode(throwRangeError(state, scope, errorMessage)); }

class StrictModeTypeErrorFunction : public InternalFunction {
private:
    StrictModeTypeErrorFunction(VM& vm, Structure* structure, const String& message)
        : InternalFunction(vm, structure)
        , m_message(message)
    {
    }

    static void destroy(JSCell*);

public:
    typedef InternalFunction Base;

    static StrictModeTypeErrorFunction* create(VM& vm, Structure* structure, const String& message)
    {
        StrictModeTypeErrorFunction* function = new (NotNull, allocateCell<StrictModeTypeErrorFunction>(vm.heap)) StrictModeTypeErrorFunction(vm, structure, message);
        function->finishCreation(vm, String());
        return function;
    }

    static EncodedJSValue JSC_HOST_CALL constructThrowTypeError(ExecState* exec)
    {
        VM& vm = exec->vm();
        auto scope = DECLARE_THROW_SCOPE(vm);
        throwTypeError(exec, scope, static_cast<StrictModeTypeErrorFunction*>(exec->jsCallee())->m_message);
        return JSValue::encode(jsNull());
    }

    static ConstructType getConstructData(JSCell*, ConstructData& constructData)
    {
        constructData.native.function = constructThrowTypeError;
        return ConstructType::Host;
    }

    static EncodedJSValue JSC_HOST_CALL callThrowTypeError(ExecState* exec)
    {
        VM& vm = exec->vm();
        auto scope = DECLARE_THROW_SCOPE(vm);
        throwTypeError(exec, scope, static_cast<StrictModeTypeErrorFunction*>(exec->jsCallee())->m_message);
        return JSValue::encode(jsNull());
    }

    static CallType getCallData(JSCell*, CallData& callData)
    {
        callData.native.function = callThrowTypeError;
        return CallType::Host;
    }

    DECLARE_INFO;

    static Structure* createStructure(VM& vm, JSGlobalObject* globalObject, JSValue prototype) 
    { 
        return Structure::create(vm, globalObject, prototype, TypeInfo(ObjectType, StructureFlags), info()); 
    }

private:
    String m_message;
};

} // namespace JSC

namespace WTF {

class PrintStream;

void printInternal(PrintStream&, JSC::ErrorType);

} // namespace WTF
