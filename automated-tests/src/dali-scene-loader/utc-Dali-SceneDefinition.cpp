/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// Enable debug log for test coverage
#define DEBUG_ENABLED 1

#include "dali-scene-loader/public-api/scene-definition.h"
#include "dali-scene-loader/public-api/utils.h"
#include <dali-test-suite-utils.h>

using namespace Dali;
using namespace Dali::SceneLoader;

int UtcDaliSceneDefinitionAddNode(void)
{
  SceneDefinition sceneDef;

  DALI_TEST_EQUAL(sceneDef.GetNodeCount(), 0u);
  auto node = new NodeDefinition();
  node->mName = "First";

  auto result = sceneDef.AddNode(std::unique_ptr<NodeDefinition>{ node });
  DALI_TEST_EQUAL(result, node);
  DALI_TEST_EQUAL(sceneDef.GetNodeCount(), 1u);
  DALI_TEST_EQUAL(sceneDef.GetNode(0), node);
  DALI_TEST_EQUAL(sceneDef.FindNode(node->mName), node);

  auto node2 = new NodeDefinition();
  node2->mName = node->mName;
  result = sceneDef.AddNode(std::unique_ptr<NodeDefinition>{ node2 });
  DALI_TEST_EQUAL(result, static_cast<NodeDefinition*>(nullptr)); // failed
  DALI_TEST_EQUAL(sceneDef.GetNodeCount(), 1u); // still
  DALI_TEST_EQUAL(sceneDef.GetNode(0), node); // still
  DALI_TEST_EQUAL(sceneDef.FindNode(node->mName), node); // still

  auto child = new NodeDefinition();
  child->mName = "Second";
  child->mParentIdx = 0;

  DALI_TEST_CHECK(node->mChildren.empty()); // these are hooked up by AddNode, base on parent idx.

  result = sceneDef.AddNode(std::unique_ptr<NodeDefinition>{ child });
  DALI_TEST_EQUAL(result, child);
  DALI_TEST_EQUAL(sceneDef.GetNodeCount(), 2u);
  DALI_TEST_EQUAL(sceneDef.GetNode(1), child);
  DALI_TEST_EQUAL(sceneDef.FindNode(child->mName), child);

  DALI_TEST_EQUAL(node->mChildren[0], 1u); // these are hooked up by AddNode, base on parent idx.

  END_TEST;
}

int UtcDaliSceneDefinitionAddRootsFail(void)
{
  SceneDefinition sceneDef;

  DALI_TEST_ASSERTION(sceneDef.AddRootNode(0), "index out of bounds");
  DALI_TEST_CHECK(sceneDef.GetRoots().empty());
  DALI_TEST_EQUAL(sceneDef.GetNodeCount(), 0u);

  END_TEST;
}

namespace
{

struct TestContext
{
  SceneDefinition sceneDef;
  NodeDefinition* root;
  NodeDefinition* childA;
  NodeDefinition* childB;

  TestContext()
  : sceneDef{},
    root{ new NodeDefinition{ "Root" }},
    childA{ new NodeDefinition{ "A" }},
    childB{ new NodeDefinition{ "B" }}
  {
    childA->mParentIdx = 0;
    childB->mParentIdx = 0;

    root = sceneDef.AddNode(std::unique_ptr<NodeDefinition>{ root });
    childA = sceneDef.AddNode(std::unique_ptr<NodeDefinition>{ childA });
    childB = sceneDef.AddNode(std::unique_ptr<NodeDefinition>{ childB });
  }
};

enum Event
{
  DEFAULT = -1,
  START,
  FINISH
};

struct NodeVisitor : NodeDefinition::IVisitor
{
  struct Visit
  {
    Event event;
    NodeDefinition* node;

    bool operator==(const Visit& other) const
    {
      return event == other.event && node == other.node;
    }
  };

  void Start(NodeDefinition& n) override
  {
    visits.push_back({ START, &n });
  }

  void Finish(NodeDefinition& n) override
  {
    visits.push_back({ FINISH, &n });
  }

  std::vector<Visit> visits;
};

struct ConstNodeVisitor : NodeDefinition::IConstVisitor
{
  struct Visit
  {
    Event  event;
    const NodeDefinition* node;

    bool operator==(const Visit& other) const
    {
      return event == other.event && node == other.node;
    }
  };

  void Start(const NodeDefinition& n) override
  {
    visits.push_back({ START, &n });
  }

  void Finish(const NodeDefinition& n) override
  {
    visits.push_back({ FINISH, &n });
  }

  std::vector<Visit> visits;
};

}

int UtcDaliSceneDefinitionAddRemoveRootNode(void)
{
  TestContext ctx;

  DALI_TEST_EQUAL(ctx.sceneDef.AddRootNode(0), 0);
  DALI_TEST_EQUAL(ctx.sceneDef.GetRoots().size(), 1u);
  DALI_TEST_EQUAL(ctx.sceneDef.GetRoots()[0], 0);

  ctx.sceneDef.RemoveRootNode(0);
  DALI_TEST_EQUAL(ctx.sceneDef.GetRoots().size(), 0);

  DALI_TEST_EQUAL(ctx.sceneDef.GetNodeCount(), 3u);

  END_TEST;
}

int UtcDaliSceneDefinitionVisit(void)
{
  TestContext ctx;

  NodeVisitor visitor;
  ctx.sceneDef.Visit(0, Customization::Choices{}, visitor);

  const NodeVisitor::Visit expected[] {
    { START, ctx.root },
    { START, ctx.childA },
    { FINISH, ctx.childA },
    { START, ctx.childB },
    { FINISH, ctx.childB },
    { FINISH, ctx.root },
  };
  DALI_TEST_CHECK(std::equal(visitor.visits.begin(), visitor.visits.end(), expected));

  END_TEST;
};

int UtcDaliSceneDefinitionConstVisit(void)
{
  TestContext ctx;

  ConstNodeVisitor visitor;
  ctx.sceneDef.Visit(0, Customization::Choices{}, visitor);

  const ConstNodeVisitor::Visit expected[] {
    { START, ctx.root },
    { START, ctx.childA },
    { FINISH, ctx.childA },
    { START, ctx.childB },
    { FINISH, ctx.childB },
    { FINISH, ctx.root },
  };
  DALI_TEST_CHECK(std::equal(visitor.visits.begin(), visitor.visits.end(), expected));

  END_TEST;
};

int UtcDaliSceneDefinitionVisitCustomized(void)
{
  TestContext ctx;

  ctx.root->mCustomization.reset(new NodeDefinition::CustomizationDefinition{ "A/B" });

  const NodeVisitor::Visit expected[] {
    { START, ctx.root },
    { START, ctx.childB },
    { FINISH, ctx.childB },
    { FINISH, ctx.root },
  };

  Customization::Choices choices;
  for (auto i : { 1, 2 })
  {
    choices.Set("A/B", i);

    NodeVisitor visitor;
    ctx.sceneDef.Visit(0, choices, visitor);

    DALI_TEST_CHECK(std::equal(visitor.visits.begin(), visitor.visits.end(), std::begin(expected)));
  }

  END_TEST;
};

int UtcDaliSceneDefinitionConstVisitCustomized(void)
{
  TestContext ctx;

  ctx.root->mCustomization.reset(new NodeDefinition::CustomizationDefinition{ "A/B" });

  const ConstNodeVisitor::Visit expected[] {
    { START, ctx.root },
    { START, ctx.childB },
    { FINISH, ctx.childB },
    { FINISH, ctx.root },
  };

  Customization::Choices choices;
  for (auto i : { 1, 2 })
  {
    choices.Set("A/B", i);

    ConstNodeVisitor visitor;
    ctx.sceneDef.Visit(0, choices, visitor);

    DALI_TEST_CHECK(std::equal(visitor.visits.begin(), visitor.visits.end(), std::begin(expected)));
  }

  END_TEST;
};

int UtcDaliSceneDefinitionGetCustomizationOptions(void)
{
  TestContext ctx;

  ctx.sceneDef.AddRootNode(0); // GetCustomizationOptions requires this.

  ctx.root->mCustomization.reset(new NodeDefinition::CustomizationDefinition{ "A/B" });
  ctx.childA->mCustomization.reset(new NodeDefinition::CustomizationDefinition{ "hello" });
  ctx.childB->mCustomization.reset(new NodeDefinition::CustomizationDefinition{ "goodbye" });

  Customization::Choices choices;
  Customization::Map options;
  ctx.sceneDef.GetCustomizationOptions(choices, options, &choices);

  DALI_TEST_EQUAL(choices.Size(), 2u);
  DALI_TEST_EQUAL(options.Size(), 2u);

  struct TestOption
  {
    std::string name;
    Customization customization;
    Customization::OptionType choice;
  };

  std::vector<TestOption> testOptions {
    { "A/B", { 2, { "Root" } }, 0 },
    { "hello", { 0, { "A" } }, 0 },
  };
  for (auto& testOption: testOptions)
  {
    auto iFind = choices.Get(testOption.name);
    DALI_TEST_EQUAL(iFind, testOption.choice);

    auto iFindOption = options.Get(testOption.name);
    DALI_TEST_CHECK(iFindOption != nullptr);
    DALI_TEST_EQUAL(iFindOption->numOptions, testOption.customization.numOptions);
    DALI_TEST_EQUAL(iFindOption->nodes.size(), testOption.customization.nodes.size());
    DALI_TEST_CHECK(std::equal(iFindOption->nodes.begin(), iFindOption->nodes.end(),
      testOption.customization.nodes.begin()));
  }

  choices.Clear();
  choices.Set("A/B", 1);
  *options.Get("A/B") = {};

  testOptions[0].choice = 1;
  testOptions[1].name = "goodbye";
  testOptions[1].customization.nodes[0] = "B";

  ctx.sceneDef.GetCustomizationOptions(choices, options, &choices);

  DALI_TEST_EQUAL(choices.Size(), 2u);
  DALI_TEST_EQUAL(options.Size(), 3u);

  for (auto& testOption: testOptions)
  {
    auto iFind = choices.Get(testOption.name);
    DALI_TEST_EQUAL(iFind, testOption.choice);

    auto iFindOption = options.Get(testOption.name);
    DALI_TEST_CHECK(iFindOption != nullptr);
    DALI_TEST_EQUAL(iFindOption->numOptions, testOption.customization.numOptions);
    DALI_TEST_EQUAL(iFindOption->nodes.size(), testOption.customization.nodes.size());
    DALI_TEST_CHECK(std::equal(iFindOption->nodes.begin(), iFindOption->nodes.end(),
      testOption.customization.nodes.begin()));
  }

  END_TEST;
}

int UtcDaliSceneDefinitionFindNode(void)
{
  TestContext ctx;

  Index result = INVALID_INDEX;
  for (auto n: { ctx.root, ctx.childA, ctx.childB })
  {
    ctx.sceneDef.FindNode(n->mName, &result);
    DALI_TEST_CHECK(result != INVALID_INDEX);
    DALI_TEST_EQUAL(ctx.sceneDef.GetNode(result), n);
  }

  END_TEST;
}

int UtcDaliSceneDefinitionConstFindNode(void)
{
  TestContext ctx;

  Index result = INVALID_INDEX;
  for (auto n: { ctx.root, ctx.childA, ctx.childB })
  {
    const_cast<const TestContext&>(ctx).sceneDef.FindNode(n->mName, &result);
    DALI_TEST_CHECK(result != INVALID_INDEX);
    DALI_TEST_EQUAL(ctx.sceneDef.GetNode(result), n);
  }

  END_TEST;
}

int UtcDaliSceneDefinitionFindNodeIndex(void)
{
  TestContext ctx;

  Index result = INVALID_INDEX;
  for (auto n: { ctx.root, ctx.childA, ctx.childB })
  {
    result = ctx.sceneDef.FindNodeIndex(*n);
    DALI_TEST_CHECK(result != INVALID_INDEX);
    DALI_TEST_EQUAL(ctx.sceneDef.GetNode(result), n);
  }

  END_TEST;
}

int UtcDaliSceneDefinitionFindNodes(void)
{
  TestContext ctx;

  std::vector<NodeDefinition*> nodes;
  auto nodeConsumer = [&nodes](NodeDefinition& nd) {
    nodes.push_back(&nd);
  };

  auto nodePredicate = [](const NodeDefinition& nd) {
    return nd.mName.length() == 1;
  };

  ctx.sceneDef.FindNodes(nodePredicate, nodeConsumer, 1);
  DALI_TEST_EQUAL(nodes.size(), 1);
  DALI_TEST_EQUAL(nodes[0]->mName, "A");
  DALI_TEST_EQUAL(nodes[0], ctx.childA);

  nodes.clear();
  ctx.sceneDef.FindNodes(nodePredicate, nodeConsumer);

  DALI_TEST_EQUAL(nodes.size(), 2);
  DALI_TEST_EQUAL(nodes[0]->mName, "A");
  DALI_TEST_EQUAL(nodes[0], ctx.childA);
  DALI_TEST_EQUAL(nodes[1]->mName, "B");
  DALI_TEST_EQUAL(nodes[1], ctx.childB);

  END_TEST;
}

int UtcDaliSceneDefinitionRemoveNode(void)
{
  TestContext ctx;
  DALI_TEST_EQUAL(ctx.sceneDef.RemoveNode("doesn't exist"), false);

  // pre-removing A
  DALI_TEST_EQUAL(ctx.sceneDef.GetNodeCount(), 3u);
  DALI_TEST_EQUAL(ctx.root->mChildren.size(), 2u);

  Index result;
  DALI_TEST_EQUAL(ctx.sceneDef.FindNode("B", &result), ctx.childB);
  DALI_TEST_EQUAL(result, 2);

  DALI_TEST_EQUAL(ctx.sceneDef.RemoveNode("A"), true);

  // post-removing A
  DALI_TEST_EQUAL(ctx.sceneDef.GetNodeCount(), 2u);

  result = 12345;
  DALI_TEST_EQUAL(ctx.sceneDef.FindNode("A", &result), static_cast<NodeDefinition*>(nullptr));
  DALI_TEST_EQUAL(result, 12345); // doesn't change

  DALI_TEST_EQUAL(ctx.sceneDef.FindNode("B", &result), ctx.childB);
  DALI_TEST_EQUAL(result, 1); // dropped

  DALI_TEST_EQUAL(ctx.root->mChildren.size(), 1u);
  DALI_TEST_EQUAL(ctx.root->mChildren[0], 1u);

  // removing root
  DALI_TEST_EQUAL(ctx.sceneDef.RemoveNode("Root"), true);
  DALI_TEST_EQUAL(ctx.sceneDef.GetNodeCount(), 0);

  END_TEST;
}

int UtcDaliSceneDefinitionReparentNode(void)
{
  TestContext ctx;

  ctx.sceneDef.ReparentNode("B", "A", 0);

  DALI_TEST_EQUAL(ctx.childB->mParentIdx, ctx.sceneDef.FindNodeIndex(*ctx.childA));
  DALI_TEST_EQUAL(ctx.childA->mChildren.size(), 1u);
  DALI_TEST_EQUAL(ctx.childA->mChildren[0], ctx.sceneDef.FindNodeIndex(*ctx.childB));

  END_TEST;
}

