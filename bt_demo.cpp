#include <iostream>
#include <chrono>
#include <vector>
#include "behaviortree_cpp_v3/action_node.h"
#include "behaviortree_cpp_v3/bt_factory.h"

using namespace std::chrono_literals;

// find ball subtree
BT::NodeStatus ballFound()
{
    std::cout << "Ball not found" << std::endl;
    return BT::NodeStatus::FAILURE;
}

class FindBall : public BT::SyncActionNode
{
public:
    explicit FindBall(const std::string &name, const BT::NodeConfiguration config) : SyncActionNode(name, config) {}

    static BT::PortsList providedPorts()
    {
        return {BT::OutputPort<std::vector<int>>("ball_location")};
    }

    BT::NodeStatus tick() override
    {
        std::this_thread::sleep_for(3s);
        std::vector<int> ballLocation{1, 2, 3};
        BT::TreeNode::setOutput("ball_location", ballLocation);
        std::cout << "Ball Found" << std::endl;
        return BT::NodeStatus::SUCCESS;
    }
};

// approach ball subtree
BT::NodeStatus ballClose(BT::TreeNode &self)
{
    BT::Optional<std::vector<int>> msg = self.getInput<std::vector<int>>("ball_location");
    // check if message exists
    if (!msg)
    {
        throw BT::RuntimeError("missing required input[message]: ", msg.error());
    }

    for (const auto positionCoordinate : msg.value())
    {
        std::cout << positionCoordinate << " ";
    }
    std::cout << "Ball location is far" << std::endl;
    return BT::NodeStatus::FAILURE;
}

class ApproachBall : public BT::SyncActionNode
{
public:
    explicit ApproachBall(const std::string &name, const BT::NodeConfiguration config) : SyncActionNode(name, config) {}

    BT::PortsList providedPorts()
    {
        return {BT::InputPort<std::vector<int>>("ball_location")};
    }

    BT::NodeStatus tick() override
    {
        BT::Optional<std::vector<int>> msg = getInput<std::vector<int>>("ball_location");
        // check if message exists
        if (!msg)
        {
            throw BT::RuntimeError("missing required input[message]: ", msg.error());
        }

        for (const auto positionCoordinate : msg.value())
        {
            std::cout << positionCoordinate << " ";
        }
        std::this_thread::sleep_for(3s);
        std::cout << "Ball Approached" << std::endl;
        return BT::NodeStatus::SUCCESS;
    }
};

// grasp ball subtree
BT::NodeStatus ballGrasped()
{
    std::cout << "Ball not grasped" << std::endl;
    return BT::NodeStatus::FAILURE;
}

class GraspBall : public BT::SyncActionNode
{
public:
    explicit GraspBall(const std::string &name) : SyncActionNode(name, {}) {}

    BT::NodeStatus tick() override
    {
        std::this_thread::sleep_for(3s);
        std::cout << "Ball Grasped" << std::endl;
        return BT::NodeStatus::SUCCESS;
    }
};

// approach bin subtree
BT::NodeStatus binClose()
{
    std::cout << "Bin not Close" << std::endl;
    return BT::NodeStatus::FAILURE;
}

class ApproachBin : public BT::SyncActionNode
{
public:
    explicit ApproachBin(const std::string &name) : SyncActionNode(name, {}) {}

    BT::NodeStatus tick() override
    {
        std::this_thread::sleep_for(3s);
        std::cout << "Ball approached" << std::endl;
        return BT::NodeStatus::SUCCESS;
    }
};

// place ball subtree
BT::NodeStatus ballPlaced()
{
    std::cout << "Ball not placed" << std::endl;
    return BT::NodeStatus::FAILURE;
}

class PlaceBall : public BT::SyncActionNode
{
public:
    explicit PlaceBall(const std::string &name) : SyncActionNode(name, {}) {}

    BT::NodeStatus tick() override
    {
        std::this_thread::sleep_for(3s);
        std::cout << "Ball Placed" << std::endl;
        return BT::NodeStatus::SUCCESS;
    }
};

// ask for help
class AskForHelp : public BT::SyncActionNode
{
public:
    explicit AskForHelp(const std::string &name) : SyncActionNode(name, {}) {}

    BT::NodeStatus tick() override
    {
        std::cout << "Asking for help. Waiting fo 10s here..." << std::endl;
        std::this_thread::sleep_for(10s);
        return BT::NodeStatus::SUCCESS;
    }
};

int main()
{

    // behavior tree factory
    BT::BehaviorTreeFactory factory;

    // register find ball subtree
    factory.registerSimpleCondition("BallFound", std::bind(ballFound));
    factory.registerNodeType<FindBall>("FindBall");

    // register approach ball subtree
    BT::PortsList say_something_ports = {BT::InputPort<std::vector<int>>("ball_location")};
    factory.registerSimpleCondition("BallClose", ballClose, say_something_ports);
    factory.registerNodeType<ApproachBall>("ApproachBall");

    // register grasp ball subtree
    factory.registerSimpleCondition("BallGrasped", std::bind(ballGrasped));
    factory.registerNodeType<GraspBall>("GraspBall");

    // register approach bin subtree
    factory.registerSimpleCondition("BinClose", std::bind(binClose));
    factory.registerNodeType<ApproachBin>("ApproachBin");

    // register place ball subtree
    factory.registerSimpleCondition("BallPlaced", std::bind(ballPlaced));
    factory.registerNodeType<PlaceBall>("PlaceBall");

    // register ask for help
    factory.registerNodeType<AskForHelp>("AskForHelp");

    // create behavior tree and tick tree
    auto tree = factory.createTreeFromFile("./../bt_tree.xml");
    tree.tickRoot();

    return 0;
}