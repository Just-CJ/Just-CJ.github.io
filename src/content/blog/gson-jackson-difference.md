---
title: Gson与Jackson在反序列化数字类型时的差异
date: 2017-10-28
description: Java序列化与反序列化Json通常会使用Gson和Jackson这两个框架，最近碰到了一个这两框架在反序列化上有些不太一样的地方。
tags:
  - Json
  - Gson
  - Jackson
  - java
categories:
  - Tech
---

Java序列化与反序列化Json通常会使用`Gson`和`Jackson`这两个框架，之前因为公司都是用的`Gson`也没太在意。最近碰到了一个这两框架在反序列化上有些不太一样的地方。

通常在Java中我们反序列化一个Json字符串是，如果我们不想定义一个具体的类，我们会把它反序列化为Java中的Map类，这时候我们其实没有提供任何Map中的value的类型的信息，序列化框架需要自己根据json字符串来判断应该反序列化为哪个类。这里我发现`Gson`在处理数字的反序列化，会默认都反序列化成`Double`，而`Jackson`则可以智能的判断。

为什么会产生这样的差异？查阅源码以后我发现，这两个框架在对JsonToken的定义上就存在差异。

```java
/**
 * Jackson的JsonToken定义
 */
public enum JsonToken
{
    NOT_AVAILABLE(null, JsonTokenId.ID_NOT_AVAILABLE),
    START_OBJECT("{", JsonTokenId.ID_START_OBJECT),
    END_OBJECT("}", JsonTokenId.ID_END_OBJECT),
    START_ARRAY("[", JsonTokenId.ID_START_ARRAY),
    END_ARRAY("]", JsonTokenId.ID_END_ARRAY),
    FIELD_NAME(null, JsonTokenId.ID_FIELD_NAME),
    VALUE_EMBEDDED_OBJECT(null, JsonTokenId.ID_EMBEDDED_OBJECT),
    VALUE_STRING(null, JsonTokenId.ID_STRING),
    VALUE_NUMBER_INT(null, JsonTokenId.ID_NUMBER_INT),
    VALUE_NUMBER_FLOAT(null, JsonTokenId.ID_NUMBER_FLOAT),
    VALUE_TRUE("true", JsonTokenId.ID_TRUE),
    VALUE_FALSE("false", JsonTokenId.ID_FALSE),
    VALUE_NULL("null", JsonTokenId.ID_NULL),
        ;
}
```

```java
/**
 * Gson的JsonToken定义
 */
public enum JsonToken {
  BEGIN_ARRAY,
  END_ARRAY,
  BEGIN_OBJECT,
  END_OBJECT,
  NAME,
  STRING,
  NUMBER,
  BOOLEAN,
  NULL,
  END_DOCUMENT
}
```

可以看到，`Jackson`在JsonToken中就将数字定义成两种类型`VALUE_NUMBER_INT`和`VALUE_NUMBER_FLOAT`，而Gson中只有一种`NUMBER`。

事实上这两个框架在解析数字时都区分了整型和浮点型，代码可以参见

- [Gson中JsonReader的peekNumber()方法](https://github.com/google/gson/blob/d9249e9c36fbb4890f6d00f90928d3ee63fc1a0f/gson/src/main/java/com/google/gson/stream/JsonReader.java#L641)
- [Jackson中ReaderBasedJsonParser的_parsePosNumber()方法](https://github.com/FasterXML/jackson-core/blob/97116ed8f5d7502340b6fb1b6887747a622fb01e/src/main/java/com/fasterxml/jackson/core/json/ReaderBasedJsonParser.java#L1237)

但是`Gson`在将其转成JsonToken时将`PEEKED_LONG`和`PEEKED_NUMBER`统一返回成了`JsonToken.NUMBER`

可以参见JsonReader中的peek()函数

```java
/**
 * Returns the type of the next token without consuming it.
 */
public JsonToken peek() throws IOException {
  int p = peeked;
  if (p == PEEKED_NONE) {
    p = doPeek();
  }

  switch (p) {
    case PEEKED_BEGIN_OBJECT:
      return JsonToken.BEGIN_OBJECT;
    case PEEKED_END_OBJECT:
      return JsonToken.END_OBJECT;
    case PEEKED_BEGIN_ARRAY:
      return JsonToken.BEGIN_ARRAY;
    case PEEKED_END_ARRAY:
      return JsonToken.END_ARRAY;
    case PEEKED_SINGLE_QUOTED_NAME:
    case PEEKED_DOUBLE_QUOTED_NAME:
    case PEEKED_UNQUOTED_NAME:
      return JsonToken.NAME;
    case PEEKED_TRUE:
    case PEEKED_FALSE:
      return JsonToken.BOOLEAN;
    case PEEKED_NULL:
      return JsonToken.NULL;
    case PEEKED_SINGLE_QUOTED:
    case PEEKED_DOUBLE_QUOTED:
    case PEEKED_UNQUOTED:
    case PEEKED_BUFFERED:
      return JsonToken.STRING;
    case PEEKED_LONG: //这里
    case PEEKED_NUMBER:
      return JsonToken.NUMBER;
    case PEEKED_EOF:
      return JsonToken.END_DOCUMENT;
    default:
      throw new AssertionError();
  }
}
```

而上层在反序列化时是以JsonToken为类型的参考依据的（当用户没有提供类型信息时），比如我们可以看`ObjectTypeAdapter`（通常我们不给类型，gson就会按Object类型来反序列化）中的反序列化

```java
public final class ObjectTypeAdapter extends TypeAdapter<Object> {
  // ...省略一些无关代码

  @Override public Object read(JsonReader in) throws IOException {
    JsonToken token = in.peek();
    switch (token) {
    case BEGIN_ARRAY:
      List<Object> list = new ArrayList<Object>();
      in.beginArray();
      while (in.hasNext()) {
        list.add(read(in));
      }
      in.endArray();
      return list;

    case BEGIN_OBJECT:
      Map<String, Object> map = new LinkedTreeMap<String, Object>();
      in.beginObject();
      while (in.hasNext()) {
        map.put(in.nextName(), read(in));
      }
      in.endObject();
      return map;

    case STRING:
      return in.nextString();

    case NUMBER: // 这里根据JsonToken的类型进行反序列化
      return in.nextDouble();

    case BOOLEAN:
      return in.nextBoolean();

    case NULL:
      in.nextNull();
      return null;

    default:
      throw new IllegalStateException();
    }
  }
  // ...省略一些无关代码
}
```

综上所述，gson其实是可以做到识别整型和浮点型，但是在上层隐藏了这个信息，不知道是出于什么考虑。
