#include "gtest/gtest.h"
#include "XMLReader.h"
#include "XMLWriter.h"
#include "StringDataSource.h"
#include "StringDataSink.h"

TEST(XMLReaderTest, SimpleXML) {
    //reading simple XML file
    std::string xmlData = "<root></root>";
    auto source = std::make_shared<CStringDataSource>(xmlData);
    CXMLReader reader(source);

    SXMLEntity entity;
    ASSERT_TRUE(reader.ReadEntity(entity));
    EXPECT_EQ(entity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(entity.DNameData, "root");

    ASSERT_TRUE(reader.ReadEntity(entity));
    EXPECT_EQ(entity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(entity.DNameData, "root");

    EXPECT_TRUE(reader.End());
}

TEST(XMLReaderTest, NestedElements) {
    std::string xmlData = "<root><child></child></root>";
    auto source = std::make_shared<CStringDataSource>(xmlData);
    CXMLReader reader(source);

    SXMLEntity entity;
    ASSERT_TRUE(reader.ReadEntity(entity));
    EXPECT_EQ(entity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(entity.DNameData, "root");

    ASSERT_TRUE(reader.ReadEntity(entity));
    EXPECT_EQ(entity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(entity.DNameData, "child");

    ASSERT_TRUE(reader.ReadEntity(entity));
    EXPECT_EQ(entity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(entity.DNameData, "child");

    ASSERT_TRUE(reader.ReadEntity(entity));
    EXPECT_EQ(entity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(entity.DNameData, "root");

    EXPECT_TRUE(reader.End());
}

TEST(XMLReaderTest, Attributes) {
    std::string xmlData = "<root attr1=\"value1\" attr2=\"value2\"></root>";
    auto source = std::make_shared<CStringDataSource>(xmlData);
    CXMLReader reader(source);

    SXMLEntity entity;
    ASSERT_TRUE(reader.ReadEntity(entity));
    EXPECT_EQ(entity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(entity.DNameData, "root");
    EXPECT_TRUE(entity.AttributeExists("attr1"));
    EXPECT_EQ(entity.AttributeValue("attr1"), "value1");
    EXPECT_TRUE(entity.AttributeExists("attr2"));
    EXPECT_EQ(entity.AttributeValue("attr2"), "value2");

    ASSERT_TRUE(reader.ReadEntity(entity));
    EXPECT_EQ(entity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(entity.DNameData, "root");

    EXPECT_TRUE(reader.End());
}

TEST(XMLWriterTest, SimpleXML) {
    auto sink = std::make_shared<CStringDataSink>();
    CXMLWriter writer(sink);

    SXMLEntity entity;
    entity.DType = SXMLEntity::EType::StartElement;
    entity.DNameData = "root";
    ASSERT_TRUE(writer.WriteEntity(entity));

    entity.DType = SXMLEntity::EType::EndElement;
    entity.DNameData = "root";
    ASSERT_TRUE(writer.WriteEntity(entity));

    ASSERT_TRUE(writer.Flush());

    std::string expectedOutput = "<root></root>";
    EXPECT_EQ(sink->String(), expectedOutput);
}

TEST(XMLWriterTest, NestedElements) {
    auto sink = std::make_shared<CStringDataSink>();
    CXMLWriter writer(sink);

    SXMLEntity entity;
    entity.DType = SXMLEntity::EType::StartElement;
    entity.DNameData = "root";
    ASSERT_TRUE(writer.WriteEntity(entity));

    entity.DType = SXMLEntity::EType::StartElement;
    entity.DNameData = "child";
    ASSERT_TRUE(writer.WriteEntity(entity));

    entity.DType = SXMLEntity::EType::EndElement;
    entity.DNameData = "child";
    ASSERT_TRUE(writer.WriteEntity(entity));

    entity.DType = SXMLEntity::EType::EndElement;
    entity.DNameData = "root";
    ASSERT_TRUE(writer.WriteEntity(entity));

    ASSERT_TRUE(writer.Flush());

    std::string expectedOutput = "<root><child></child></root>";
    EXPECT_EQ(sink->String(), expectedOutput);
}

TEST(XMLWriterTest, Attributes) {
    auto sink = std::make_shared<CStringDataSink>();
    CXMLWriter writer(sink);

    SXMLEntity entity;
    entity.DType = SXMLEntity::EType::StartElement;
    entity.DNameData = "root";
    entity.DAttributes = {{"attr1", "value1"}, {"attr2", "value2"}};
    ASSERT_TRUE(writer.WriteEntity(entity));

    entity.DType = SXMLEntity::EType::EndElement;
    entity.DNameData = "root";
    ASSERT_TRUE(writer.WriteEntity(entity));

    ASSERT_TRUE(writer.Flush());

    std::string expectedOutput = "<root attr1=\"value1\" attr2=\"value2\"></root>";
    EXPECT_EQ(sink->String(), expectedOutput);
}
