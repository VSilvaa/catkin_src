////******************************************************************************************************
  // Todos os includes
////******************************************************************************************************


#include <ros/ros.h>
#include <moveit/move_group_interface/move_group.h>
// MoveIt!
#include <moveit/robot_model_loader/robot_model_loader.h>
#include <moveit/robot_model/robot_model.h>
#include <moveit/robot_state/robot_state.h>
#include <geometry_msgs/Pose.h>

#include <moveit_msgs/PlanningScene.h>
#include <moveit_msgs/AttachedCollisionObject.h>
#include <moveit_msgs/GetStateValidity.h>
#include <moveit_msgs/DisplayRobotState.h>
#include <moveit/robot_state/conversions.h>
#include <pluginlib/class_loader.h>
#include <moveit/planning_interface/planning_interface.h>
#include <moveit/planning_scene/planning_scene.h>
#include <moveit/kinematic_constraints/utils.h>
#include <moveit/planning_pipeline/planning_pipeline.h>
#include <moveit_msgs/DisplayTrajectory.h>
#include <eigen_conversions/eigen_msg.h>
#include <sensor_msgs/JointState.h>
#include <tf/transform_broadcaster.h>
//#include <moveit/joint_model_group.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>
#include <moveit_msgs/CollisionObject.h>
#include <iostream>
#include <moveit_msgs/GetPositionIK.h>
////******************************************************************************************************
  // Todos os exertos de tutoriais 
////******************************************************************************************************

#if 1
int main(int argc, char **argv)
{

  ros::init (argc, argv, "all");
  ros::AsyncSpinner spinner(1);
  spinner.start();
  
////******************************************************************************************************
  // Kinematic_model
////******************************************************************************************************

  // We will start by instantiating a
  // `RobotModelLoader`_
  // object, which will look up
  // the robot description on the ROS parameter server and construct a
  // :moveit_core:`RobotModel` for us to use.
  //
  // .. _RobotModelLoader: http://docs.ros.org/api/moveit_ros_planning/html/classrobot__model__loader_1_1RobotModelLoader.html

  robot_model_loader::RobotModelLoader robot_model_loader("robot_description");
  robot_model::RobotModelPtr kinematic_model = robot_model_loader.getModel();
  ROS_INFO("Model frame: %s", kinematic_model->getModelFrame().c_str());
  
    // Using the :moveit_core:`RobotModel`, we can construct a
  // :moveit_core:`RobotState` that maintains the configuration
  // of the robot. We will set all joints in the state to their
  // default values. We can then get a
  // :moveit_core:`JointModelGroup`, which represents the robot
  // model for a particular group, e.g. the "right_arm" of the PR2
  // robot.
  robot_state::RobotStatePtr kinematic_state(new robot_state::RobotState(kinematic_model));
  kinematic_state->setToDefaultValues();
  const robot_state::JointModelGroup* joint_model_group = kinematic_model->getJointModelGroup("manipulator");

  const std::vector<std::string> &joint_names = joint_model_group->getJointModelNames();
  
  ////******************************************************************************************************
  // Get Joint Values
////******************************************************************************************************
  // We can retreive the current set of joint values stored in the state for the right arm.
  
  ROS_INFO("\n Get Joint Values..............................................");
  std::vector<double> joint_values;
  kinematic_state->copyJointGroupPositions(joint_model_group, joint_values);
  for(std::size_t i = 0; i < joint_names.size(); ++i)
  {
    ROS_INFO("Joint %s: %f", joint_names[i].c_str(), joint_values[i]);
  }


////******************************************************************************************************
  // Joint Limits
////******************************************************************************************************
  // setJointGroupPositions() does not enforce joint limits by itself, but a call to enforceBounds() will do it.
  /* Set one joint in the right arm outside its joint limit */
  ROS_INFO("\n Joint Limits..............................................");
  joint_values[0] = 1.57;
  kinematic_state->setJointGroupPositions(joint_model_group, joint_values);

  /* Check whether any joint is outside its joint limits */
  ROS_INFO_STREAM("Current state is " << (kinematic_state->satisfiesBounds() ? "valid" : "not valid"));

  /* Enforce the joint limits for this state and check again*/
  kinematic_state->enforceBounds();
  ROS_INFO_STREAM("Current state is " << (kinematic_state->satisfiesBounds() ? "valid" : "not valid"));



////******************************************************************************************************
  // Forward Kinematics
////******************************************************************************************************
  // Now, we can compute forward kinematics for a set of random joint
  // values. Note that we would like to find the pose of the
  // "r_wrist_roll_link" which is the most distal link in the
  // "right_arm" of the robot.
  
    ROS_INFO("\n Forward Kinematics..............................................");
  kinematic_state->setToRandomPositions(joint_model_group);
  const Eigen::Affine3d &end_effector_state = kinematic_state->getGlobalLinkTransform("tool0");

  /* Print end-effector pose. Remember that this is in the model frame */
  ROS_INFO_STREAM("Translation: " << end_effector_state.translation());
  ROS_INFO_STREAM("Rotation: " << end_effector_state.rotation());




////******************************************************************************************************
  // Inverse Kinematics
////******************************************************************************************************
  // We can now solve inverse kinematics (IK) for the right arm of the
  // PR2 robot. To solve IK, we will need the following:
  //  * The desired pose of the end-effector (by default, this is the last link in the "right_arm" chain): end_effector_state that we computed in the step above.
  //  * The number of attempts to be made at solving IK: 5
  //  * The timeout for each attempt: 0.1 s
    ROS_INFO("\n Inverse Kinematics..............................................");  
  bool found_ik = kinematic_state->setFromIK(joint_model_group, end_effector_state, 10, 0.1);

  // Now, we can print out the IK solution (if found):
  if (found_ik)
  {
    kinematic_state->copyJointGroupPositions(joint_model_group, joint_values);
    for(std::size_t i=0; i < joint_names.size(); ++i)
    {
      ROS_INFO("Joint %s: %f", joint_names[i].c_str(), joint_values[i]);
    }
  }
  else
  {
    ROS_INFO("Did not find IK solution");
  }




////******************************************************************************************************
  // Get the Jacobian
////******************************************************************************************************
  // We can also get the Jacobian from the :moveit_core:`RobotState`.
      ROS_INFO("\n Get the Jacobian..............................................");  
  Eigen::Vector3d reference_point_position(0.0,0.0,0.0);
  Eigen::MatrixXd jacobian;
  kinematic_state->getJacobian(joint_model_group, kinematic_state->getLinkModel(joint_model_group->getLinkModelNames().back()),
                               reference_point_position,
                               jacobian);
  ROS_INFO_STREAM("Jacobian: " << jacobian);
  
  
}
#endif




















































#if 0			//move_1
int main(int argc, char **argv)
{
  ros::init(argc, argv, "lesson_move_group");

  // start a background "spinner", so our node can process ROS messages
  //  - this lets us know when the move is completed
  ros::AsyncSpinner spinner(1);
  spinner.start();

  moveit::planning_interface::MoveGroup group("manipulator");
  group.setRandomTarget();
  group.move();
}

#endif





















































#if 0      //move_2
int main(int argc, char **argv)
{
  ros::init(argc, argv, "lesson_move_group");

  // start a background "spinner", so our node can process ROS messages
  //  - this lets us know when the move is completed
  ros::AsyncSpinner spinner(1);
  spinner.start();

  moveit::planning_interface::MoveGroup group("manipulator");
  group.setNamedTarget("straight_up");
  group.move();
}

#endif





















































#if 0      //move_4 ..............................................................estava1
int main(int argc, char **argv)
{
  ros::init(argc, argv, "lesson_move_group");

  // start a background "spinner", so our node can process ROS messages
  //  - this lets us know when the move is completed
  ros::AsyncSpinner spinner(1);
  spinner.start();

  moveit::planning_interface::MoveGroup group("manipulator");

  double angle =M_PI;
  Eigen::Quaterniond quat1(Eigen::AngleAxis<double>(double(angle), Eigen::Vector3d::UnitZ()));

  Eigen::Affine3d pose = Eigen::Translation3d(0.3, 0.10, 0.50)
                         * Eigen::Quaterniond(quat1.w(), quat1.x(), quat1.y(), quat1.z()); //0.707
                         //                     W       X       Y       Z
  
  group.setPoseTarget(pose);
  group.move();
}

#endif





















































#if 0      //move_6
int main(int argc, char **argv)
{
  ros::init (argc, argv, "planning_scene_ros_api_tutorial");
  ros::AsyncSpinner spinner(1); 
  spinner.start();

  ros::NodeHandle node_handle;
  ros::Duration sleep_time(5.0);
  sleep_time.sleep();
  sleep_time.sleep();         
  
  
  // ROS API
 //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// The ROS API to the planning scene publisher is through a topic interface
// using "diffs". A planning scene diff is the difference between the current 
// planning scene (maintained by the move_group node) and the new planning 
// scene desired by the user. 

// Advertise the required topic
 //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Note that this topic may need to be remapped in the launch file 
  ros::Publisher planning_scene_diff_publisher = node_handle.advertise<moveit_msgs::PlanningScene>("planning_scene", 1);
  while(planning_scene_diff_publisher.getNumSubscribers() < 1)
  {
    ros::WallDuration sleep_t(0.5);
    sleep_t.sleep();
  }
  
  
  
  
  // Define the attached object message
 //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// We will use this message to add or 
// subtract the object from the world 
// and to attach the object to the robot
  moveit_msgs::AttachedCollisionObject attached_object;
  attached_object.link_name = "tool0";
  /* The header must contain a valid TF frame*/
  attached_object.object.header.frame_id = "tool0";
  /* The id of the object */
  attached_object.object.id = "box";

  /* A default pose */
  geometry_msgs::Pose pose;
  pose.orientation.w = 1.0;

  /* Define a box to be attached */
  shape_msgs::SolidPrimitive primitive;
  primitive.type = primitive.BOX;
  primitive.dimensions.resize(3);
  primitive.dimensions[0] = 0.02;
  primitive.dimensions[1] = 0.02;
  primitive.dimensions[2] = 0.1;

  attached_object.object.primitives.push_back(primitive);
  attached_object.object.primitive_poses.push_back(pose);

// Note that attaching an object to the robot requires 
// the corresponding operation to be specified as an ADD operation
  attached_object.object.operation = attached_object.object.ADD;


// Add an object into the environment
 //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Add the object into the environment by adding it to 
// the set of collision objects in the "world" part of the 
// planning scene. Note that we are using only the "object" 
// field of the attached_object message here.
  ROS_INFO("Adding the object into the world at the location of the right wrist.");
  moveit_msgs::PlanningScene planning_scene;
  planning_scene.world.collision_objects.push_back(attached_object.object);
  planning_scene.is_diff = true;
  planning_scene_diff_publisher.publish(planning_scene);
  sleep_time.sleep();     
  
  
  // Attach an object to the robot
 //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// When the robot picks up an object from the environment, we need to 
// "attach" the object to the robot so that any component dealing with 
// the robot model knows to account for the attached object, e.g. for
// collision checking.

// Attaching an object requires two operations
//  * Removing the original object from the environment
//  * Attaching the object to the robot

  /* First, define the REMOVE object message*/
  moveit_msgs::CollisionObject remove_object;
  remove_object.id = "box";
  remove_object.header.frame_id = "odom_combined";
  remove_object.operation = remove_object.REMOVE;

// Note how we make sure that the diff message contains no other
// attached objects or collisions objects by clearing those fields
// first.
  /* Carry out the REMOVE + ATTACH operation */
  ROS_INFO("Attaching the object to the right wrist and removing it from the world.");
  planning_scene.world.collision_objects.clear();
  planning_scene.world.collision_objects.push_back(remove_object);
  planning_scene.robot_state.attached_collision_objects.push_back(attached_object);
  planning_scene_diff_publisher.publish(planning_scene);

  sleep_time.sleep();
  
  
  
  
  // Detach an object from the robot
 //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Detaching an object from the robot requires two operations
//  * Detaching the object from the robot
//  * Re-introducing the object into the environment

  /* First, define the DETACH object message*/
  moveit_msgs::AttachedCollisionObject detach_object;
  detach_object.object.id = "box";
  detach_object.link_name = "tool0";
  detach_object.object.operation = attached_object.object.REMOVE;

// Note how we make sure that the diff message contains no other
// attached objects or collisions objects by clearing those fields
// first.
  /* Carry out the DETACH + ADD operation */
  ROS_INFO("Detaching the object from the robot and returning it to the world.");
  planning_scene.robot_state.attached_collision_objects.clear();
  planning_scene.robot_state.attached_collision_objects.push_back(detach_object);
  planning_scene.world.collision_objects.clear();
  planning_scene.world.collision_objects.push_back(attached_object.object);
  planning_scene_diff_publisher.publish(planning_scene);

  sleep_time.sleep();
  
  
  // REMOVE THE OBJECT FROM THE COLLISION WORLD
 //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Removing the object from the collision world just requires
// using the remove object message defined earlier. 
// Note, also how we make sure that the diff message contains no other
// attached objects or collisions objects by clearing those fields
// first.
  ROS_INFO("Removing the object from the world.");
  planning_scene.robot_state.attached_collision_objects.clear();
  planning_scene.world.collision_objects.clear();
  planning_scene.world.collision_objects.push_back(remove_object);
  planning_scene_diff_publisher.publish(planning_scene);

  ros::shutdown();
  return 0;
  
}//END MAIN

#endif























































#if 0      //move_7 ERRO	ERRO	ERRO	ERRO	ERRO	ERRO	ERRO		ERRO	ERRO	ERRO	ERRO	ERRO	ERRO	ERRO	ERRO	ERRO	ERROERRO	ERRO
int main(int argc, char **argv)
{

 ros::init (argc, argv, "move_group_tutorial");
  ros::AsyncSpinner spinner(1);
  spinner.start();
  ros::NodeHandle node_handle("~");

  // BEGIN_TUTORIAL
  // Start
  // ^^^^^
  // Setting up to start using a planner is pretty easy. Planners are 
  // setup as plugins in MoveIt! and you can use the ROS pluginlib
  // interface to load any planner that you want to use. Before we 
  // can load the planner, we need two objects, a RobotModel 
  // and a PlanningScene.
  // We will start by instantiating a
  // `RobotModelLoader`_
  // object, which will look up
  // the robot description on the ROS parameter server and construct a
  // :moveit_core:`RobotModel` for us to use.
  //
  // .. _RobotModelLoader: http://docs.ros.org/api/moveit_ros_planning/html/classrobot__model__loader_1_1RobotModelLoader.html
  robot_model_loader::RobotModelLoader robot_model_loader("robot_description");
  robot_model::RobotModelPtr robot_model = robot_model_loader.getModel();


  // Using the :moveit_core:`RobotModel`, we can construct a
  // :planning_scene:`PlanningScene` that maintains the state of 
  // the world (including the robot). 
  planning_scene::PlanningScenePtr planning_scene(new planning_scene::PlanningScene(robot_model));

  // We will now construct a loader to load a planner, by name. 
  // Note that we are using the ROS pluginlib library here.
  boost::scoped_ptr<pluginlib::ClassLoader<planning_interface::PlannerManager> > planner_plugin_loader;
  planning_interface::PlannerManagerPtr planner_instance;
  std::string planner_plugin_name;
  ROS_INFO("ERRO AQUI!! 1111111111111111111.\n");
  // We will get the name of planning plugin we want to load
  // from the ROS param server, and then load the planner
  // making sure to catch all exceptions.
  if (!node_handle.getParam("planning_plugin", planner_plugin_name))
    ROS_FATAL_STREAM("Could not find planner plugin name");
  try
  {
    planner_plugin_loader.reset(new pluginlib::ClassLoader<planning_interface::PlannerManager>("moveit_core", "planning_interface::PlannerManager"));
    ROS_INFO("ERRO AQUI!! 222222222222222222222.");
  }
  catch(pluginlib::PluginlibException& ex)
  {
    ROS_FATAL_STREAM("Exception while creating planning plugin loader " << ex.what());
  }
  try
  {
    planner_instance.reset(planner_plugin_loader->createUnmanagedInstance(planner_plugin_name));
    if (!planner_instance->initialize(robot_model, node_handle.getNamespace()))
      ROS_FATAL_STREAM("Could not initialize planner instance");
    ROS_INFO_STREAM("Using planning interface '" << planner_instance->getDescription() << "'");
  }
  catch(pluginlib::PluginlibException& ex)
  {
    const std::vector<std::string> &classes = planner_plugin_loader->getDeclaredClasses();
    std::stringstream ss;
    for (std::size_t i = 0 ; i < classes.size() ; ++i)
      ss << classes[i] << " ";
    ROS_ERROR_STREAM("Exception while loading planner '" << planner_plugin_name << "': " << ex.what() << std::endl
                     << "Available plugins: " << ss.str());
  }

  /* Sleep a little to allow time to startup rviz, etc. */
  ros::WallDuration sleep_time(15.0);
  sleep_time.sleep();

  
}//END MAIN

//http://docs.ros.org/hydro/api/pr2_moveit_tutorials/html/planning/src/doc/motion_planning_api_tutorial.html
//https://github.com/ros-planning/moveit_pr2/blob/indigo-devel/pr2_moveit_tutorials/planning/src/motion_planning_api_tutorial.cpp
#endif






















































#if 0 				//move_8 ERRO	ERRO	ERRO	ERRO	ERRO	ERRO	ERRO		ERRO	ERRO	ERRO	ERRO	ERRO	ERRO	ERRO	ERRO	ERRO	ERROERRO	ERRO
int main(int argc, char **argv)
{
  ros::init (argc, argv, "move_group_tutorial");
  ros::AsyncSpinner spinner(1);
  spinner.start();
  ros::NodeHandle node_handle("~");

  // BEGIN_TUTORIAL
  // Start
  // ^^^^^
  // Setting up to start using a planning pipeline is pretty easy. 
  // Before we can load the planner, we need two objects, a RobotModel 
  // and a PlanningScene.
  // We will start by instantiating a
  // `RobotModelLoader`_
  // object, which will look up
  // the robot description on the ROS parameter server and construct a
  // :moveit_core:`RobotModel` for us to use.
  //
  // .. _RobotModelLoader: http://docs.ros.org/api/moveit_ros_planning/html/classrobot__model__loader_1_1RobotModelLoader.html

  robot_model_loader::RobotModelLoader robot_model_loader("robot_description");

  robot_model::RobotModelPtr robot_model = robot_model_loader.getModel();

  // Using the :moveit_core:`RobotModel`, we can construct a
  // :planning_scene:`PlanningScene` that maintains the state of 
  // the world (including the robot). 
  planning_scene::PlanningScenePtr planning_scene(new planning_scene::PlanningScene(robot_model));

  // We can now setup the 
  // `PlanningPipeline`_
  // object, which will use the ROS param server 
  // to determine the set of request adapters and the 
  // planning plugin to use
  planning_pipeline::PlanningPipelinePtr planning_pipeline(new planning_pipeline::PlanningPipeline(robot_model, node_handle, "planning_plugin", "request_adapters"));

  /* Sleep a little to allow time to startup rviz, etc. */
  ros::WallDuration sleep_time(5.0);
  sleep_time.sleep();
          ROS_INFO("1^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
  // Pose Goal
  // ^^^^^^^^^
  // We will now create a motion plan request for the right arm of the PR2
  // specifying the desired pose of the end-effector as input.
  planning_interface::MotionPlanRequest req;
  planning_interface::MotionPlanResponse res;
  geometry_msgs::PoseStamped pose;
  pose.header.frame_id = "base_link";
  pose.pose.position.x = 0.0;
  pose.pose.position.y = 0.0;
  pose.pose.position.z = 0.1;
  pose.pose.orientation.w = 0.0;

  // A tolerance of 0.01 m is specified in position
  // and 0.01 radians in orientation
  std::vector<double> tolerance_pose(3, 0.1);
  std::vector<double> tolerance_angle(3, 0.1);

  // We will create the request as a constraint using a helper function available 
  // from the 
  // `kinematic_constraints`_
  // package.
  //
  // .. _kinematic_constraints: http://docs.ros.org/api/moveit_core/html/namespacekinematic__constraints.html#a88becba14be9ced36fefc7980271e132
  req.group_name = "manipulator";
  moveit_msgs::Constraints pose_goal = kinematic_constraints::constructGoalConstraints("tool0", pose, tolerance_pose, tolerance_angle);
  req.goal_constraints.push_back(pose_goal);
          ROS_INFO("2^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
  // Now, call the pipeline and check whether planning was successful.
  planning_pipeline->generatePlan(planning_scene, req, res);
            ROS_INFO("3^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
  /* Check that the planning was successful */
  if(res.error_code_.val != res.error_code_.SUCCESS)
  {
    ROS_ERROR("Could not compute plan successfully");
    return 0;
  }

  // Visualize the result
  // ^^^^^^^^^^^^^^^^^^^^
  ros::Publisher display_publisher = node_handle.advertise<moveit_msgs::DisplayTrajectory>("/move_group/display_planned_path", 1, true);
  moveit_msgs::DisplayTrajectory display_trajectory;

  /* Visualize the trajectory */
  ROS_INFO("Visualizing the trajectory");
  moveit_msgs::MotionPlanResponse response;
  res.getMessage(response);

  display_trajectory.trajectory_start = response.trajectory_start;
  display_trajectory.trajectory.push_back(response.trajectory);
  display_publisher.publish(display_trajectory);

  sleep_time.sleep();
  
}

//http://docs.ros.org/hydro/api/pr2_moveit_tutorials/html/planning/src/doc/planning_pipeline_tutorial.html#id1
//https://github.com/ros-planning/moveit_pr2/blob/indigo-devel/pr2_moveit_tutorials/planning/src/planning_pipeline_tutorial.cpp

#endif




















































#if 0 				//motion_planning_api
int main(int argc, char **argv)
{
  ros::init (argc, argv, "move_group_tutorial");
  ros::AsyncSpinner spinner(1);
  spinner.start();
  ros::NodeHandle node_handle("~");

  // ^^^^^
  // Setting up to start using a planner is pretty easy. Planners are 
  // setup as plugins in MoveIt! and you can use the ROS pluginlib
  // interface to load any planner that you want to use. Before we 
  // can load the planner, we need two objects, a RobotModel 
  // and a PlanningScene.
  // We will start by instantiating a
  // `RobotModelLoader`_
  // object, which will look up
  // the robot description on the ROS parameter server and construct a
  // :moveit_core:`RobotModel` for us to use.
  //
  // .. _RobotModelLoader: http://docs.ros.org/api/moveit_ros_planning/html/classrobot__model__loader_1_1RobotModelLoader.html
  robot_model_loader::RobotModelLoader robot_model_loader("robot_description");
  robot_model::RobotModelPtr robot_model = robot_model_loader.getModel();

  
  // Using the :moveit_core:`RobotModel`, we can construct a
  // :planning_scene:`PlanningScene` that maintains the state of 
  // the world (including the robot). 
  planning_scene::PlanningScenePtr planning_scene(new planning_scene::PlanningScene(robot_model));

  // We will now construct a loader to load a planner, by name. 
  // Note that we are using the ROS pluginlib library here.
  boost::scoped_ptr<pluginlib::ClassLoader<planning_interface::PlannerManager> > planner_plugin_loader;
  planning_interface::PlannerManagerPtr planner_instance;
  std::string planner_plugin_name;

  // We will get the name of planning plugin we want to load
  // from the ROS param server, and then load the planner
  // making sure to catch all exceptions.
  if (!node_handle.getParam("planning_plugin", planner_plugin_name))
    ROS_FATAL_STREAM("Could not find planner plugin name");
  try
  {
    planner_plugin_loader.reset(new pluginlib::ClassLoader<planning_interface::PlannerManager>("moveit_core", "planning_interface::PlannerManager"));
  }
  catch(pluginlib::PluginlibException& ex)
  {
    ROS_FATAL_STREAM("Exception while creating planning plugin loader " << ex.what());
  }
  try
  {
    planner_instance.reset(planner_plugin_loader->createUnmanagedInstance(planner_plugin_name));
    if (!planner_instance->initialize(robot_model, node_handle.getNamespace()))
      ROS_FATAL_STREAM("Could not initialize planner instance");
    ROS_INFO_STREAM("Using planning interface '" << planner_instance->getDescription() << "'");
  }
  catch(pluginlib::PluginlibException& ex)
  {
    const std::vector<std::string> &classes = planner_plugin_loader->getDeclaredClasses();
    std::stringstream ss;
    for (std::size_t i = 0 ; i < classes.size() ; ++i)
      ss << classes[i] << " ";
    ROS_ERROR_STREAM("Exception while loading planner '" << planner_plugin_name << "': " << ex.what() << std::endl
                     << "Available plugins: " << ss.str());
  }

  /* Sleep a little to allow time to startup rviz, etc. */
  ros::WallDuration sleep_time(15.0);
  sleep_time.sleep();




//////******************************************************************************************************
//  // Pose Goal
//////******************************************************************************************************
//  // We will now create a motion plan request for the right arm of the PR2
//  // specifying the desired pose of the end-effector as input.
//  planning_interface::MotionPlanRequest req;
//  planning_interface::MotionPlanResponse res;
//  geometry_msgs::PoseStamped pose;
//  pose.header.frame_id = "tool0";
//  pose.pose.position.x = 0.75;
//  pose.pose.position.y = 0.0;
//  pose.pose.position.z = 0.0;
//  pose.pose.orientation.w = 1.0;

//  // A tolerance of 0.01 m is specified in position
//  // and 0.01 radians in orientation
//  std::vector<double> tolerance_pose(3, 0.01);
//  std::vector<double> tolerance_angle(3, 0.01);

//  // We will create the request as a constraint using a helper function available 
//  // from the 
//  // `kinematic_constraints`_
//  // package.
//  //
//  // .. _kinematic_constraints: http://docs.ros.org/api/moveit_core/html/namespacekinematic__constraints.html#a88becba14be9ced36fefc7980271e132
//  req.group_name = "manipulator";
//  moveit_msgs::Constraints pose_goal = kinematic_constraints::constructGoalConstraints("tool0", pose, tolerance_pose, tolerance_angle);
//  req.goal_constraints.push_back(pose_goal);

//  // We now construct a planning context that encapsulate the scene,
//  // the request and the response. We call the planner using this 
//  // planning context
//  planning_interface::PlanningContextPtr context = planner_instance->getPlanningContext(planning_scene, req, res.error_code_);
//  context->solve(res);
//  if(res.error_code_.val != res.error_code_.SUCCESS)
//  {
//    ROS_ERROR("Could not compute plan successfully");
//    return 0;
//  }



//////******************************************************************************************************
//  // Visualize the result
//////******************************************************************************************************
//  ros::Publisher display_publisher = node_handle.advertise<moveit_msgs::DisplayTrajectory>("/move_group/display_planned_path", 1, true);
//  moveit_msgs::DisplayTrajectory display_trajectory;

//  /* Visualize the trajectory */
//  ROS_INFO("Visualizing the trajectory");
//  moveit_msgs::MotionPlanResponse response;
//  res.getMessage(response);

//  display_trajectory.trajectory_start = response.trajectory_start;
//  display_trajectory.trajectory.push_back(response.trajectory);
//  display_publisher.publish(display_trajectory);

//  sleep_time.sleep();





//////******************************************************************************************************
//  // Joint Space Goals
//////******************************************************************************************************
//  /* First, set the state in the planning scene to the final state of the last plan */
//  robot_state::RobotState& robot_state = planning_scene->getCurrentStateNonConst();
//  planning_scene->setCurrentState(response.trajectory_start);
//  const robot_state::JointModelGroup* joint_model_group = robot_state.getJointModelGroup("manipulator");
//  robot_state.setJointGroupPositions(joint_model_group, response.trajectory.joint_trajectory.points.back().positions);

//  // Now, setup a joint space goal
//  robot_state::RobotState goal_state(robot_model);
//  std::vector<double> joint_values(7, 0.0);
//  joint_values[0] = -2.0;
//  joint_values[3] = -0.2;
//  joint_values[5] = -0.15;
//  goal_state.setJointGroupPositions(joint_model_group, joint_values);
//  moveit_msgs::Constraints joint_goal = kinematic_constraints::constructGoalConstraints(goal_state, joint_model_group);
//  req.goal_constraints.clear();
//  req.goal_constraints.push_back(joint_goal);

//  // Call the planner and visualize the trajectory
//  /* Re-construct the planning context */
//  context = planner_instance->getPlanningContext(planning_scene, req, res.error_code_);
//  /* Call the Planner */
//  context->solve(res);
//  /* Check that the planning was successful */
//  if(res.error_code_.val != res.error_code_.SUCCESS)
//  {
//    ROS_ERROR("Could not compute plan successfully");
//    return 0;
//  }
//  /* Visualize the trajectory */
//  ROS_INFO("Visualizing the trajectory");
//  res.getMessage(response);
//  display_trajectory.trajectory_start = response.trajectory_start;
//  display_trajectory.trajectory.push_back(response.trajectory);

//  /* Now you should see two planned trajectories in series*/
//  display_publisher.publish(display_trajectory);

//  /* We will add more goals. But first, set the state in the planning 
//     scene to the final state of the last plan */
//  robot_state.setJointGroupPositions(joint_model_group, response.trajectory.joint_trajectory.points.back().positions);

//  /* Now, we go back to the first goal*/
//  req.goal_constraints.clear();
//  req.goal_constraints.push_back(pose_goal);
//  context = planner_instance->getPlanningContext(planning_scene, req, res.error_code_);
//  context->solve(res);
//  res.getMessage(response);
//  display_trajectory.trajectory.push_back(response.trajectory);
//  display_publisher.publish(display_trajectory);







//////******************************************************************************************************
//  // Adding Path Constraints
//////******************************************************************************************************
//  // Let's add a new pose goal again. This time we will also add a path constraint to the motion.
//  /* Let's create a new pose goal */
//  pose.pose.position.x = 0.65;
//  pose.pose.position.y = -0.2;
//  pose.pose.position.z = -0.1;
//  moveit_msgs::Constraints pose_goal_2 = kinematic_constraints::constructGoalConstraints("tool0", pose, tolerance_pose, tolerance_angle);
//  /* First, set the state in the planning scene to the final state of the last plan */
//  robot_state.setJointGroupPositions(joint_model_group, response.trajectory.joint_trajectory.points.back().positions);
//  /* Now, let's try to move to this new pose goal*/
//  req.goal_constraints.clear();
//  req.goal_constraints.push_back(pose_goal_2);

//  /* But, let's impose a path constraint on the motion.
//     Here, we are asking for the end-effector to stay level*/
//  geometry_msgs::QuaternionStamped quaternion;
//  quaternion.header.frame_id = "torso_lift_link";
//  quaternion.quaternion.w = 1.0;
//  req.path_constraints = kinematic_constraints::constructGoalConstraints("tool0", quaternion);

//  // Imposing path constraints requires the planner to reason in the space of possible positions of the end-effector
//  // (the workspace of the robot)
//  // because of this, we need to specify a bound for the allowed planning volume as well;
//  // Note: a default bound is automatically filled by the WorkspaceBounds request adapter (part of the OMPL pipeline,
//  // but that is not being used in this example).
//  // We use a bound that definitely includes the reachable space for the arm. This is fine because sampling is not done in this volume
//  // when planning for the arm; the bounds are only used to determine if the sampled configurations are valid.
//  req.workspace_parameters.min_corner.x = req.workspace_parameters.min_corner.y = req.workspace_parameters.min_corner.z = -2.0;
//  req.workspace_parameters.max_corner.x = req.workspace_parameters.max_corner.y = req.workspace_parameters.max_corner.z =  2.0;

//  // Call the planner and visualize all the plans created so far.
//  context = planner_instance->getPlanningContext(planning_scene, req, res.error_code_);
//  context->solve(res);
//  res.getMessage(response);
//  display_trajectory.trajectory.push_back(response.trajectory);
//  // Now you should see four planned trajectories in series
//  display_publisher.publish(display_trajectory);

//  //END_TUTORIAL
//  sleep_time.sleep();
//  ROS_INFO("Done");
//  planner_instance.reset();

  return 0;
}
#endif




















































#if 0 				//move_3
////******************************************************************************************************
////Movimento incremental junta 1
////******************************************************************************************************

//int main(int argc, char **argv)
//{
//  ros::init(argc, argv, "lesson_move_group");

//  // start a background "spinner", so our node can process ROS messages
//  //  - this lets us know when the move is completed
//  ros::AsyncSpinner spinner(1);
//  spinner.start();
//  
//	double posit=0.00;
//	
//for(int n=0;n<4;n++){
//	  std::map<std::string, double> joints;
//	  joints["joint_1"] = posit;
//	  joints["joint_2"] =  0.00;
//	  joints["joint_3"] =  0.00;
//	  joints["joint_4"] =  0.00;
//	  joints["joint_5"] =  0.00;
//	  joints["joint_6"] = 0.00;

//	  moveit::planning_interface::MoveGroup group("manipulator");
//	  group.setJointValueTarget(joints);
//	  std::cout<<"PASSEI\n";
//	  group.move();
//	  
//	  std::cout<<"posit1="<<posit<<"\n";
//	  posit=posit+0.40;

//	std::cout<<"posit2="<<posit;
//		std::cout<<"VALOR="<<n<<"\n";
//	
//}
//std::cout<<"ACABEI";
//return 0;
//}




//******************************************************************************************************
//Movimento juntas entre duas posições
//******************************************************************************************************
int main(int argc, char **argv)
{
  ros::init(argc, argv, "Criado_Por_Tiago");
	moveit::planning_interface::MoveGroup group("manipulator");
  // start a background "spinner", so our node can process ROS messages
  //  - this lets us know when the move is completed
  ros::AsyncSpinner spinner(1);
  spinner.start();
  
      ros::NodeHandle n;
  
	double posit=0.00;
	double max_velocity_scaling_factor=0.11;
	double max_acceleration_scaling_factor=1;
	double valores;
	
//for(int n=0;n<2;n++){
			group.setMaxVelocityScalingFactor (max_velocity_scaling_factor);
//			group.setMaxAccelerationScalingFactor(max_acceleration_scaling_factor);

		  std::map<std::string, double> joints1;
		  std::map<std::string, double> joints2;
		  joints1["joint_1"] = 0.00;
		  joints1["joint_2"] =  0.00;
		  joints1["joint_3"] =  0.00;
		  joints1["joint_4"] =  0.00;
		  joints1["joint_5"] =  0.00;
		  joints1["joint_6"] = 0.00;

		  valores=group.getPlanningTime ();
		  std::cout<<"TIME,TIME="<<valores<<"\n";
		  
		  group.setJointValueTarget(joints1);
		  std::cout<<"PASSEI ANTES\n";
		  group.move();


//		  std::cout<<"PASSEI DEPOIS\n";
//		
//		  joints2["joint_1"] = 0.00;
//		  joints2["joint_2"] =  0.00;
//		  joints2["joint_3"] =  0.00;
//		  joints2["joint_4"] =  0.00;
//		  joints2["joint_5"] =  0.00;
//		  joints2["joint_6"] = 0.50;

//		  group.setJointValueTarget(joints2);
//		  std::cout<<"PASSEI\n";
//		  group.move();
	
//}

    
    

//        // update transform
//        // (moving in a circle with radius)
//        odom_trans.header.stamp = ros::Time::now();
//        odom_trans.transform.translation.x = cos(angle);
//        odom_trans.transform.translation.y = sin(angle);
//        odom_trans.transform.translation.z = 0.0;
////        odom_trans.transform.rotation = tf::createQuaternionMsgFromYaw(angle);

//        //send the joint state and transform
//        joint_pub.publish(joint_state);
//        broadcaster.sendTransform(odom_trans);

//	// Create new robot state
//        POSjoint_1 += link_2_inc;
////        if (arm2_arm1<-1.5 || arm2_arm1>1.5) arm2_arm1_inc *= -1;
////		  arm1_armbase += arm1_armbase_inc;
////        if (arm1_armbase>1.2 || arm1_armbase<-1.0) arm1_armbase_inc *= -1;
////        base_arm += base_arm_inc;
////        if (base_arm>1. || base_arm<-1.0) base_arm_inc *= -1;
////        gripper += gripper_inc;
////        if (gripper<0 || gripper>1) gripper_inc *= -1;
//		
//		  angle += degree/4;

//        // This will adjust as needed per iteration
//        loop_rate.sleep();
//    }
    
std::cout<<"ACABEI";
}


////
//// User defined constraints can also be specified to the PlanningScene
//// class. This is done by specifying a callback using the
//// setStateFeasibilityPredicate function. Here's a simple example of a
//// user-defined callback that checks whether the "r_shoulder_pan" of
//// the PR2 robot is at a positive or negative angle:
//bool userCallback(const robot_state::RobotState &kinematic_state, bool verbose)
//{
//  const double* joint_values = kinematic_state.getJointPositions("joint_1");
//  return (joint_values[0] > 0.0);
//}
//// END_SUB_TUTORIAL

//int main(int argc, char **argv)
//{
//  ros::init (argc, argv, "right_FANUC2_kinematics");
//  ros::AsyncSpinner spinner(1);
//  spinner.start();

//// BEGIN_TUTORIAL
//// 
//// Setup
//// ^^^^^
//// 
//// The :planning_scene:`PlanningScene` class can be easily setup and
//// configured using a :moveit_core:`RobotModel` or a URDF and
//// SRDF. This is, however, not the recommended way to instantiate a
//// PlanningScene. The :planning_scene_monitor:`PlanningSceneMonitor`
//// is the recommended method to create and maintain the current
//// planning scene (and is discussed in detail in the next tutorial)
//// using data from the robot's joints and the sensors on the robot. In
//// this tutorial, we will instantiate a PlanningScene class directly,
//// but this method of instantiation is only intended for illustration.

//  robot_model_loader::RobotModelLoader robot_model_loader("robot_description");
//  robot_model::RobotModelPtr kinematic_model = robot_model_loader.getModel();
//  planning_scene::PlanningScene planning_scene(kinematic_model);

////^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//// Collision Checking
////^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
////
//// Self-collision checking
////^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
////
//// The first thing we will do is check whether the robot in its
//// current state is in *self-collision*, i.e. whether the current
//// configuration of the robot would result in the robot's parts
//// hitting each other. To do this, we will construct a
//// :collision_detection_struct:`CollisionRequest` object and a
//// :collision_detection_struct:`CollisionResult` object and pass them
//// into the collision checking function. Note that the result of
//// whether the robot is in self-collision or not is contained within
//// the result. Self collision checking uses an *unpadded* version of
//// the robot, i.e. it directly uses the collision meshes provided in
//// the URDF with no extra padding added on.
//      ROS_INFO_STREAM("*******************************Collision Checking**************\n");
//  collision_detection::CollisionRequest collision_request;
//  collision_detection::CollisionResult collision_result;
//  planning_scene.checkSelfCollision(collision_request, collision_result);
//  ROS_INFO_STREAM("Test 1: Current state is "
//                  << (collision_result.collision ? "in" : "not in")
//                  << " self collision");
//                  
//     // Change the state
////^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
////
//// Now, let's change the current state of the robot. The planning
//// scene maintains the current state internally. We can get a
//// reference to it and change it and then check for collisions for the
//// new robot configuration. Note in particular that we need to clear
//// the collision_result before making a new collision checking
//// request.

//  robot_state::RobotState& current_state = planning_scene.getCurrentStateNonConst();
//  current_state.setToRandomPositions();
////	current_state.position.x = -0.0628377;
////	current_state.position.y = -0.621449;
////	current_state.position.z = 0.603022;
////	current_state.orientation.x =-0.9;
////	current_state.orientation.y =-0.964875;
////	current_state.orientation.z =0.256041;
////	current_state.orientation.w =-0.0428377;
//  collision_result.clear();
//  planning_scene.checkSelfCollision(collision_request, collision_result);
//  ROS_INFO_STREAM("Test 2: Current state is "
//                  << (collision_result.collision ? "in" : "not in")
//                  << " self collision");           
//                  
//                  
// // Checking for a group
////^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
////
//// Now, we will do collision checking only for the right_arm of the
//// PR2, i.e. we will check whether there are any collisions between
//// the right arm and other parts of the body of the robot. We can ask
//// for this specifically by adding the group name "right_arm" to the
//// collision request.
//      ROS_INFO_STREAM("*******************************Checking for a group**************\n");
//  collision_request.group_name = "manipulator";
//  current_state.setToRandomPositions();
//  collision_result.clear();
//  planning_scene.checkSelfCollision(collision_request, collision_result);
//  ROS_INFO_STREAM("Test 3: Current state is "
//                  << (collision_result.collision ? "in" : "not in")
//                  << " self collision");  
//                  
//                  
//// Getting Contact Information
////^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
////
//// First, manually set the right arm to a position where we know
//// internal (self) collisions do happen. Note that this state is now
//// actually outside the joint limits of the PR2, which we can also
//// check for directly.
//      ROS_INFO_STREAM("*******************************Getting Contact Information**************\n");
//  std::vector<double> joint_values;
//  const robot_model::JointModelGroup* joint_model_group =
//    current_state.getJointModelGroup("manipulator");
//  current_state.copyJointGroupPositions(joint_model_group, joint_values);
//  joint_values[1] = 3.57; //hard-coded since we know collisions will happen here
//  current_state.setJointGroupPositions(joint_model_group, joint_values);
//  ROS_INFO_STREAM("Current state is "
//                  << (current_state.satisfiesBounds(joint_model_group) ? "valid" : "not valid"));

//// Now, we can get contact information for any collisions that might
//// have happened at a given configuration of the right arm. We can ask
//// for contact information by filling in the appropriate field in the
//// collision request and specifying the maximum number of contacts to
//// be returned as a large number.

//  collision_request.contacts = true;
//  collision_request.max_contacts = 1000;

////

//  collision_result.clear();
//  planning_scene.checkSelfCollision(collision_request, collision_result);
//  ROS_INFO_STREAM("Test 4: Current state is "
//                  << (collision_result.collision ? "in" : "not in")
//                  << " self collision");
//  collision_detection::CollisionResult::ContactMap::const_iterator it;
//  for(it = collision_result.contacts.begin();
//      it != collision_result.contacts.end();
//      ++it)
//  {
//    ROS_INFO("Contact between: %s and %s",
//             it->first.first.c_str(),
//             it->first.second.c_str());
//  }
//  
//  
//  
//  
//// Modifying the Allowed Collision Matrix
////^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
////
//// The :collision_detection_class:`AllowedCollisionMatrix` (ACM)
//// provides a mechanism to tell the collision world to ignore
//// collisions between certain object: both parts of the robot and
//// objects in the world. We can tell the collision checker to ignore
//// all collisions between the links reported above, i.e. even though
//// the links are actually in collision, the collision checker will
//// ignore those collisions and return not in collision for this
//// particular state of the robot.
////
//// Note also in this example how we are making copies of both the
//// allowed collision matrix and the current state and passing them in
//// to the collision checking function.
//      ROS_INFO_STREAM("************Modifying the Allowed Collision Matrix**************\n");
//  collision_detection::AllowedCollisionMatrix acm = planning_scene.getAllowedCollisionMatrix();
//  robot_state::RobotState copied_state = planning_scene.getCurrentState();

//  collision_detection::CollisionResult::ContactMap::const_iterator it2;
//  for(it2 = collision_result.contacts.begin();
//      it2 != collision_result.contacts.end();
//      ++it2)
//  {
//    acm.setEntry(it2->first.first, it2->first.second, true);
//  }
//  collision_result.clear();
//  planning_scene.checkSelfCollision(collision_request, collision_result, copied_state, acm);
//  ROS_INFO_STREAM("Test 5: Current state is "
//                  << (collision_result.collision ? "in" : "not in")
//                  << " self collision");

//  // Full Collision Checking
////^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//  //
//  // While we have been checking for self-collisions, we can use the
//  // checkCollision functions instead which will check for both
//  // self-collisions and for collisions with the environment (which is
//  // currently empty).  This is the set of collision checking
//  // functions that you will use most often in a planner. Note that
//  // collision checks with the environment will use the padded version
//  // of the robot. Padding helps in keeping the robot further away
//  // from obstacles in the environment.*/
//        ROS_INFO_STREAM("**********Full Collision Checking**************\n");
//  collision_result.clear();
//  planning_scene.checkCollision(collision_request, collision_result, copied_state, acm);
//  ROS_INFO_STREAM("Test 6: Current state is "
//                  << (collision_result.collision ? "in" : "not in")
//                  << " self collision");







// // Constraint Checking
////^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//  //
//  // The PlanningScene class also includes easy to use function calls
//  // for checking constraints. The constraints can be of two types:
//  // (a) constraints chosen from the
//  // :kinematic_constraints:`KinematicConstraint` set:
//  // i.e. :kinematic_constraints:`JointConstraint`,
//  // :kinematic_constraints:`PositionConstraint`,
//  // :kinematic_constraints:`OrientationConstraint` and
//  // :kinematic_constraints:`VisibilityConstraint` and (b) user
//  // defined constraints specified through a callback. We will first
//  // look at an example with a simple KinematicConstraint.
//  //
//  // Checking Kinematic Constraints
////^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//  //
//  // We will first define a simple position and orientation constraint
//  // on the end-effector of the robot. Note the
//  // use of convenience functions for filling up the constraints
//  // (these functions are found in the :moveit_core_files:`utils.h<utils_8h>` file from the
//  // kinematic_constraints directory in moveit_core).

//  std::string end_effector_name = joint_model_group->getLinkModelNames().back();

//  geometry_msgs::PoseStamped desired_pose;
//  desired_pose.pose.orientation.w = 1.0;
//  desired_pose.pose.position.x = 0.75;
//  desired_pose.pose.position.y = -0.185;
//  desired_pose.pose.position.z = 1.3;
//  desired_pose.header.frame_id = "base_link";
//  moveit_msgs::Constraints goal_constraint =
//    kinematic_constraints::constructGoalConstraints(end_effector_name, desired_pose);

//// Now, we can check a state against this constraint using the
//// isStateConstrained functions in the PlanningScene class.

//  copied_state.setToRandomPositions();
//  copied_state.update();
//  bool constrained = planning_scene.isStateConstrained(copied_state, goal_constraint);
//  ROS_INFO_STREAM("Test 7: Random state is "
//                  << (constrained ? "constrained" : "not constrained"));

//// There's a more efficient way of checking constraints (when you want
//// to check the same constraint over and over again, e.g. inside a
//// planner). We first construct a KinematicConstraintSet which
//// pre-processes the ROS Constraints messages and sets it up for quick
//// processing.

//  kinematic_constraints::KinematicConstraintSet kinematic_constraint_set(kinematic_model);
//  kinematic_constraint_set.add(goal_constraint, planning_scene.getTransforms());
//  bool constrained_2 = planning_scene.isStateConstrained(copied_state, kinematic_constraint_set);
//  ROS_INFO_STREAM("Test 8: Random state is "
//                  << (constrained_2 ? "constrained" : "not constrained"));

//// There's a direct way to do this using the KinematicConstraintSet
//// class.

//  kinematic_constraints::ConstraintEvaluationResult constraint_eval_result = kinematic_constraint_set.decide(copied_state);
//  ROS_INFO_STREAM("Test 9: Random state is "
//                  << (constraint_eval_result.satisfied ? "constrained" : "not constrained"));

//// User-defined constraints
////^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
////
//// CALL_SUB_TUTORIAL userCallback

//// Now, whenever isStateFeasible is called, this user-defined callback
//// will be called.

//  planning_scene.setStateFeasibilityPredicate(userCallback);
//  bool state_feasible = planning_scene.isStateFeasible(copied_state);
//  ROS_INFO_STREAM("Test 10: Random state is "
//                  << (state_feasible ? "feasible" : "not feasible"));

//// Whenever isStateValid is called, three checks are conducted: (a)
//// collision checking (b) constraint checking and (c) feasibility
//// checking using the user-defined callback.

//  bool state_valid =
//    planning_scene.isStateValid(copied_state, kinematic_constraint_set, "right_arm");
//  ROS_INFO_STREAM("Test 11: Random state is "
//                  << (state_valid ? "valid" : "not valid"));

//// Note that all the planners available through MoveIt! and OMPL will
//// currently perform collision checking, constraint checking and
//// feasibility checking using user-defined callbacks.

//  ros::shutdown();
//  return 0;
//                  
//}
#endif




















































#if 0 				//move_5

int main(int argc, char **argv)
{
  ros::init(argc, argv, "TIAGO_move_group");
  moveit::planning_interface::PlanningSceneInterface planning_scene_interface;

	ros::NodeHandle node_handle; 

  // start a background "spinner", so our node can process ROS messages
  //  - this lets us know when the move is completed
  ros::AsyncSpinner spinner(1);
  spinner.start();

  moveit::planning_interface::MoveGroup group("manipulator");

group.setPlanningTime(45);
 group.setEndEffectorLink("tool0");



//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//Informativo
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  // We can print the name of the reference frame for this robot.
  ROS_INFO("Reference frame: %s", group.getPlanningFrame().c_str());
  
  // We can also print the name of the end-effector link for this group.
  ROS_INFO("Reference frame: %s", group.getEndEffectorLink().c_str());
  
   // (Optional) Create a publisher for visualizing plans in Rviz.
	ros::Publisher display_publisher = node_handle.advertise<moveit_msgs::DisplayTrajectory>("/move_group/display_planned_path", 1, true);
	moveit_msgs::DisplayTrajectory display_trajectory;


sleep(1.0);

  // We can plan a motion for this group to a desired pose for the end-effector.
  geometry_msgs::Pose pose;
 pose.position.x = -0.0628377;
 pose.position.y = -0.581449;
 pose.position.z = 0.753022;
pose.orientation.x =-0.9;
pose.orientation.y =-0.964875;
 pose.orientation.z =0.256041;
 pose.orientation.w =-0.0428377;

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   // Visualizing plans
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    // Now, we call the planner to compute the plan
  // and visualize it.
  // Note that we are just planning, not asking move_group 
  // to actually move the robot.
  moveit::planning_interface::MoveGroup::Plan My_First_Plan;
  bool success = group.plan(My_First_Plan);

  ROS_INFO("Visualizing plan 1 (pose goal) %s",success?"":"FAILED");    
  /* Sleep to give Rviz time to visualize the plan. */
  sleep(5.0);
	

  if (1)
  {
    ROS_INFO("Visualizing plan 1 (again)");    
    display_trajectory.trajectory_start = My_First_Plan.start_state_;
    display_trajectory.trajectory.push_back(My_First_Plan.trajectory_);
    display_publisher.publish(display_trajectory);
    /* Sleep to give Rviz time to visualize the plan. */
    sleep(5.0);
  }





//  group.setPoseTarget(pose);
//  group.move();






////  // Planning to a joint-space goal 
//////^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
////  
////  // First get the current set of joint values for the group.
////  std::vector<double> group_variable_values;
////  group.getCurrentState()->copyJointGroupPositions(group.getCurrentState()->getRobotModel()->getJointModelGroup(group.getName()), group_variable_values);
////   // Now, let's modify one of the joints, plan to the new joint
////  // space goal and visualize the plan.
////  group_variable_values[0] = -0.25;  
////  group.setJointValueTarget(group_variable_values);
////  success = group.plan(My_First_Plan);

////  ROS_INFO("Visualizing plan 2 (joint space goal) %s",success?"":"FAILED");
////  /* Sleep to give Rviz time to visualize the plan. */
////  sleep(5.0);
////  
////  //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
////  group.move();
  
  
  
  
  
//////^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
////  // Cartesian Paths 
//////^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
////  // You can plan a cartesian path directly by specifying a list of waypoints 
////  // for the end-effector to go through. Note that we are starting 
////  // from the new start state above.  The initial pose (start state) does not
////  // need to be added to the waypoint list.
////  std::vector<geometry_msgs::Pose> waypoints;

//////  geometry_msgs::Pose target_pose3 = start_pose2;
////geometry_msgs::Pose target_pose3 = pose;
//////  target_pose3.position.x += 0.1;
////  target_pose3.position.z += 0.3;
////  waypoints.push_back(target_pose3);  // up and out
//// group.move();
//////  sleep(5.0);
//////  target_pose3.position.x -= 0.1;
//////  waypoints.push_back(target_pose3);  // left
////////  target_pose3.position.z -= 0.1;
////////  target_pose3.position.y += 0.1;
//////  target_pose3.position.x += 0.1;
//////  waypoints.push_back(target_pose3);  // down and right (back to start)

//////  // We want the cartesian path to be interpolated at a resolution of 1 cm
//////  // which is why we will specify 0.01 as the max step in cartesian
//////  // translation.  We will specify the jump threshold as 0.0, effectively
//////  // disabling it.
//////  moveit_msgs::RobotTrajectory trajectory;
//////  double fraction = group.computeCartesianPath(waypoints,
//////                                               0.001,  // eef_step
//////                                               0.0,   // jump_threshold
//////                                               trajectory);

//////  ROS_INFO("Visualizing plan 4 (cartesian path) (%.2f%% acheived)",
//////        fraction * 100.0);    
//////  /* Sleep to give Rviz time to visualize the plan. */
//////  group.move();
//////  sleep(15.0);




//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Adding/Removing Objects and Attaching/Detaching Objects
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  // First, we will define the collision object message.
  moveit_msgs::CollisionObject collision_object;
  collision_object.header.frame_id = group.getPlanningFrame();

  /* The id of the object is used to identify it. */
  collision_object.id = "box1";
  collision_object.id = "box2";

  /* Define a box to add to the world. */
  shape_msgs::SolidPrimitive primitive;
  primitive.type = primitive.BOX;
  primitive.dimensions.resize(3);
  primitive.dimensions[0] = 0.1;
  primitive.dimensions[1] = 0.1;
  primitive.dimensions[2] = 0.5;

  /* A pose for the box (specified relative to frame_id) */
  geometry_msgs::Pose box_pose;
  box_pose.orientation.w = 1.0;
  box_pose.position.x =  0.1;
  box_pose.position.y = -0.6;
  box_pose.position.z =  0.4;
  
  geometry_msgs::Pose box_pose2;
  box_pose2.orientation.w = 1.0;
  box_pose2.position.x =  0.1;
  box_pose2.position.y = 1.6;
  box_pose2.position.z =  0.4;

  collision_object.primitives.push_back(primitive);
  collision_object.primitive_poses.push_back(box_pose);
  collision_object.operation = collision_object.ADD;
  
  collision_object.primitives.push_back(primitive);
  collision_object.primitive_poses.push_back(box_pose2);
  collision_object.operation = collision_object.ADD;


  std::vector<moveit_msgs::CollisionObject> collision_objects;  
  collision_objects.push_back(collision_object);  

  // Now, let's add the collision object into the world
  ROS_INFO("Add an object into the world");  
  planning_scene_interface.addCollisionObjects(collision_objects);
  
  /* Sleep so we have time to see the object in RViz */
  sleep(2.0);

  // Planning with collision detection can be slow.  Lets set the planning time
  // to be sure the planner has enough time to plan around the box.  10 seconds
  // should be plenty.
  group.setPlanningTime(10.0);


  // Now when we plan a trajectory it will avoid the obstacle
  group.setStartState(*group.getCurrentState());
  group.setPoseTarget(pose);
  success = group.plan(My_First_Plan);

  ROS_INFO("Visualizing plan 5 (pose goal move around box) %s",
    success?"":"FAILED");
  /* Sleep to give Rviz time to visualize the plan. */
  sleep(10.0);
  

  // Now, let's attach the collision object to the robot.
  ROS_INFO("Attach the object to the robot");  
  group.attachObject(collision_object.id);  
  /* Sleep to give Rviz time to show the object attached (different color). */
  sleep(4.0);


  // Now, let's detach the collision object from the robot.
  ROS_INFO("Detach the object from the robot");  
  group.detachObject(collision_object.id);  
  /* Sleep to give Rviz time to show the object detached. */
  sleep(4.0);
  
//  pose;
 group.move();


//  // Now, let's remove the collision object from the world.
//  ROS_INFO("Remove the object from the world");  
//    sleep(14.0);
//  std::vector<std::string> object_ids;
//  object_ids.push_back(collision_object.id);  
//  planning_scene_interface.removeCollisionObjects(object_ids);
//  /* Sleep to give Rviz time to show the object is no longer there. */
//  sleep(4.0);

spinner.stop();
ros::shutdown(); 
return 0;

}

#endif





















































#if 0 				//move_teste vvvvvvvv

int main(int argc, char **argv)
{

	ros::init(argc, argv, "seven_dof_arm_planner");
	ros::NodeHandle node_handle;
	ros::AsyncSpinner spinner(1);



	spinner.start();

	moveit::planning_interface::MoveGroup group("manipulator");

	moveit::planning_interface::PlanningSceneInterface planning_scene_interface;

	group.setEndEffectorLink("tool0");
	
	geometry_msgs::PoseStamped robot_pose;
	robot_pose = group.getCurrentPose();

	geometry_msgs::Pose current_position;
	current_position = robot_pose.pose;

	/*Retrive position and orientation */
	geometry_msgs::Point exact_pose = current_position.position;
	geometry_msgs::Quaternion exact_orientation = current_position.orientation;

	std::cout<<"Robot position : "<<exact_pose.x<<"\t"<<exact_pose.y<<"\t"<<exact_pose.z<<std::endl;
	std::cout<<"Robot Orientation : "<<exact_orientation.x<<"\t"<<exact_orientation.y<<"\t"<<exact_orientation.z<<"\t"<<exact_orientation.w<<std::endl;

	



	ROS_INFO("Reference frame : %s",group.getPlanningFrame().c_str());

	ROS_INFO("Reference frame : %s",group.getEndEffectorLink().c_str());
	
	
	sleep(4.0);
 // (Optional) Create a publisher for visualizing plans in Rviz.
  ros::Publisher display_publisher = node_handle.advertise<moveit_msgs::DisplayTrajectory>("/move_group/display_planned_path", 1, true);
  moveit_msgs::DisplayTrajectory display_trajectory;


////******************************************************************************************************
////Planning to a Pose goal
////******************************************************************************************************
  // We can plan a motion for this group to a desired pose for the end-effector.
  geometry_msgs::Pose target_pose1;
  geometry_msgs::Pose target_pose2;
  geometry_msgs::Pose target_pose3;
  geometry_msgs::Pose target_pose4;
  geometry_msgs::Pose target_pose5;
  
//  Eigen::Affine3d target_pose1;
//  Eigen::Affine3d target_pose2;
//  Eigen::Affine3d target_pose3;

	 target_pose1.position.x = 0.415008;
	 target_pose1.position.y = 0.0;
	 target_pose1.position.z = 0.505;
//	 
	 // Orientation
    double angle =M_PI;
    Eigen::Quaterniond quat(Eigen::AngleAxis<double>(double(angle), Eigen::Vector3d::UnitZ()));
    
    target_pose1.orientation.x = quat.x();
    target_pose1.orientation.y = quat.y();
    target_pose1.orientation.z = quat.z();
    target_pose1.orientation.w = quat.w();

//	Eigen::Affine3d target_pose1 = Eigen::Translation3d(0.415008, 0.0, 0.505)
//         				  			* Eigen::Quaterniond(quat.w(), quat.x(), quat.y(), quat.z());
//      				       			 //                     W       X       Y          Z
    
    
    angle =M_PI;
    Eigen::Quaterniond quat1(Eigen::AngleAxis<double>(double(angle), Eigen::Vector3d::UnitY()));
    
//    	Eigen::Affine3d target_pose2 = Eigen::Translation3d(0.415008, 0.0, 0.505)
//         				  			* Eigen::Quaterniond(quat1.w(), quat1.x(), quat1.y(), quat1.z());
//      				       			 //                     W       X       Y          Z
    
     target_pose2.position.x=target_pose1.position.x;
	 target_pose2.position.y = target_pose1.position.y;
	 target_pose2.position.z = target_pose1.position.z;
	 
	 target_pose2.orientation.x = quat1.x();
    target_pose2.orientation.y = quat1.y();
    target_pose2.orientation.z = quat1.z();
    target_pose2.orientation.w = quat1.w();
	 


	target_pose3.position.x = 0.575008;
	 target_pose3.position.y = -0.25;
	 target_pose3.position.z = 0.312;

 target_pose3.orientation.x = quat1.x();
    target_pose3.orientation.y = quat1.y();
    target_pose3.orientation.z = quat1.z();
    target_pose3.orientation.w = quat1.w();
    
//    Eigen::Affine3d target_pose3 = Eigen::Translation3d(0.575008, -0.25, 0.312)
//         				  			* Eigen::Quaterniond(quat1.w(), quat1.x(), quat1.y(), quat1.z());
//      				       			 //                     W       X       Y          Z
  
    group.setPoseTarget(target_pose1);
    group.setPoseTarget(target_pose2);
        group.setPoseTarget(target_pose3);

  // Now, we call the planner to compute the plan
  // and visualize it.
  // Note that we are just planning, not asking move_group 
  // to actually move the robot.
  moveit::planning_interface::MoveGroup::Plan my_plan;
  bool success = group.plan(my_plan);
 

  ROS_INFO("Visualizing plan 1 (pose goal) %s",success?"":"FAILED");    
  /* Sleep to give Rviz time to visualize the plan. */
  sleep(5.0);



//////******************************************************************************************************
//////Visualizing plans
//////******************************************************************************************************
  // Now that we have a plan we can visualize it in Rviz.  This is not
  // necessary because the group.plan() call we made above did this
  // automatically.  But explicitly publishing plans is useful in cases that we
  // want to visualize a previously created plan.
//  if (1)
//  {
//    ROS_INFO("Visualizing plan 1 (again)");    
//    display_trajectory.trajectory_start = my_plan.start_state_;
//    display_trajectory.trajectory.push_back(my_plan.trajectory_);
//    display_publisher.publish(display_trajectory);
//    /* Sleep to give Rviz time to visualize the plan. */
//    sleep(5.0);
//  }
//  
//  
//  // Moving to a pose goal
//////******************************************************************************************************
   group.move();
   
   //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  // Cartesian Paths 
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  // You can plan a cartesian path directly by specifying a list of waypoints 
  // for the end-effector to go through. Note that we are starting 
  // from the new start state above.  The initial pose (start state) does not
  // need to be added to the waypoint list.
  
    robot_model_loader::RobotModelLoader robot_model_loader("robot_description");
  robot_model::RobotModelPtr kinematic_model = robot_model_loader.getModel();
  
//  end_eff_joint_groups = kinematic_model->getEndEffectors();
//  const std::string& parent_group_name = end_eff_joint_groups.at(i)->getName();
//   group_names.push_back(parent_group_name);
  
  std::vector<geometry_msgs::Pose> waypoints;

	target_pose4.position.x = 0.575008;
	 target_pose4.position.y = -0.24;
	 target_pose4.position.z = 0.312;

 target_pose4.orientation.x = quat1.x();
    target_pose4.orientation.y = quat1.y();
    target_pose4.orientation.z = quat1.z();
    target_pose4.orientation.w = quat1.w();
    
    
    
    
    


//   Eigen::Affine3d target_pose4 = Eigen::Translation3d(0.575008, -0.25, 0.312)
//         				  			* Eigen::Quaterniond(quat1.w(), quat1.x(), quat1.y(), quat1.z());
//      				       			 //                     W       X       Y          Z
  waypoints.push_back(target_pose4);   //up and out
// group.move();
//  sleep(5.0);
////    	target_pose4.position.x = 0.575008;
	 target_pose4.position.z +=0.11;
////	 target_pose4.position.z = 0.312;

//// target_pose4.orientation.x = quat1.x();
////    target_pose4.orientation.y = quat1.y();
////    target_pose4.orientation.z = quat1.z();
////    target_pose4.orientation.w = quat1.w();
//   Eigen::Affine3d target_pose5 = Eigen::Translation3d(0.575008, 0.15, 0.312)
//         				  			* Eigen::Quaterniond(quat1.w(), quat1.x(), quat1.y(), quat1.z());
//      				       			 //                     W       X       Y          Z
  waypoints.push_back(target_pose4);  // left
//  waypoints.push_back(target_pose3);  // down and right (back to start)
 target_pose4.position.y =0.11;
  waypoints.push_back(target_pose4);   //up and out
  // We want the cartesian path to be interpolated at a resolution of 1 cm
  // which is why we will specify 0.01 as the max step in cartesian
  // translation.  We will specify the jump threshold as 0.0, effectively
  // disabling it.
  moveit_msgs::RobotTrajectory trajectory;
  double fraction = group.computeCartesianPath(waypoints,
                                               0.001,  // eef_step
                                               0.0,   // jump_threshold
                                               trajectory);
//  robot_trajectory::RobotTrajectory rt(kinematic_model, group_names[selected_plan_group]);

  ROS_INFO("Visualizing plan 4 (cartesian path) (%.2f%% acheived)",
        fraction * 100.0);    
  /* Sleep to give Rviz time to visualize the plan. */
//  group.move();
  sleep(15.0);
   
   

      				       			 
//     group.setPoseTarget(target_pose4);
	  group.move();
   
//	 	ros::shutdown();
//	return 0;

//  ros::init(argc, argv, "move_group_interface");
//  ros::NodeHandle node_handle;  
//  ros::AsyncSpinner spinner(1);
//  spinner.start();


//  /* This sleep is ONLY to allow Rviz to come up */
////  sleep(20.0);
//  
//  // Setup
//  // ^^^^^
//  // 
//  // The :move_group_interface:`MoveGroup` class can be easily 
//  // setup using just the name
//  // of the group you would like to control and plan for.
//  moveit::planning_interface::MoveGroup group("manipulator");

//  // We will use the :planning_scene_interface:`PlanningSceneInterface`
//  // class to deal directly with the world.
//  moveit::planning_interface::PlanningSceneInterface planning_scene_interface;  

//  // (Optional) Create a publisher for visualizing plans in Rviz.
//  ros::Publisher display_publisher = node_handle.advertise<moveit_msgs::DisplayTrajectory>("/move_group/display_planned_path", 1, true);
//  moveit_msgs::DisplayTrajectory display_trajectory;







//////******************************************************************************************************
//////Getting Basic Information
//////******************************************************************************************************
//  // We can print the name of the reference frame for this robot.
//  ROS_INFO("Reference frame: %s", group.getPlanningFrame().c_str());
//  
//  // We can also print the name of the end-effector link for this group.
//  ROS_INFO("Reference frame: %s", group.getEndEffectorLink().c_str());


//group.setPlanningTime(45);
// group.setEndEffectorLink("tool0");



//////******************************************************************************************************
//////Planning to a Pose goal
//////******************************************************************************************************
//  // We can plan a motion for this group to a desired pose for the 
//  // end-effector.
//  geometry_msgs::Pose target_pose1;
//  

////  target_pose1.position.x = 0.50;
////  target_pose1.position.y = -0.200;
////  target_pose1.position.z = 0.755; //0.67
//  
//   target_pose1.position.x = 0.1828377;
// target_pose1.position.y = -0.581449;
// target_pose1.position.z = 0.853022;
//target_pose1.orientation.x =-0.9;
//target_pose1.orientation.y =-0.964875;
// target_pose1.orientation.z =0.256041;
// target_pose1.orientation.w =-0.0428377;
////  
////  // RotationAngle is in radians
////  float ValorX = 0;
////  float ValorY = 0;
////  float ValorZ = 0;
////  float ValorW = M_PI / 2;
//  
//	
//  
////  target_pose1.orientation.x = std::sin(ValorX);
////  target_pose1.orientation.y =  std::sin(ValorY);
////  target_pose1.orientation.z =  std::sin(ValorZ);
////  target_pose1.orientation.w =  std::cos(ValorW);
////  
////  target_pose1.orientation.w =  sqrt(0.5);
////  target_pose1.orientation.x =  sqrt(0.5);
////  target_pose1.orientation.y =  0;
////  target_pose1.orientation.z =  0;

////    Eigen::Affine3d target_pose1 = Eigen::Translation3d(0.30, -0.50, 0.685)
////                        		 * Eigen::Quaterniond(0.707, 0,  0.707+0.000, 0.000);
//                         //                		      W       X       Y       Z
//  
//  group.setPoseTarget(target_pose1);


//  // Now, we call the planner to compute the plan
//  // and visualize it.
//  // Note that we are just planning, not asking move_group 
//  // to actually move the robot.
//  moveit::planning_interface::MoveGroup::Plan my_plan;
//  bool success = group.plan(my_plan);

//  ROS_INFO("Visualizing plan 1 (pose goal) %s",success?"":"FAILED");    
//  /* Sleep to give Rviz time to visualize the plan. */
//  sleep(5.0);

////////******************************************************************************************************
////////Visualizing plans
////////******************************************************************************************************
//  // Now that we have a plan we can visualize it in Rviz.  This is not
//  // necessary because the group.plan() call we made above did this
//  // automatically.  But explicitly publishing plans is useful in cases that we
//  // want to visualize a previously created plan.
//  if (1)
//  {
//    ROS_INFO("Visualizing plan 1 (again)");    
//    display_trajectory.trajectory_start = my_plan.start_state_;
//    display_trajectory.trajectory.push_back(my_plan.trajectory_);
//    display_publisher.publish(display_trajectory);
//    /* Sleep to give Rviz time to visualize the plan. */
//    sleep(5.0);
//  }
////  
////  
////  // Moving to a pose goal
////////******************************************************************************************************
////   group.move();
//   
////   group.execute(my_plan);


////////////////////sleep(2);
//////******************************************************************************************************
////  // Planning to a joint-space goal 
////////******************************************************************************************************
////  //
////  // Let's set a joint space goal and move towards it.  This will replace the
////  // pose target we set above.
////  //
////  // First get the current set of joint values for the group.
////  std::vector<double> group_variable_values;
////  group.getCurrentState()->copyJointGroupPositions(group.getCurrentState()->getRobotModel()->getJointModelGroup(group.getName()), group_variable_values);
////  
////  // Now, let's modify one of the joints, plan to the new joint
////  // space goal and visualize the plan.
////  group_variable_values[0] = -1.0;  
////  group.setJointValueTarget(group_variable_values);
////  success = group.plan(my_plan);

////  ROS_INFO("Visualizing plan 2 (joint joint joint joint joint space goal) %s",success?"":"FAILED");
////  
////  ROS_INFO("joint joint joint joint joint joint joint joint");
////  
////    group.move();
////  
////  /* Sleep to give Rviz time to visualize the plan. */
////  sleep(5.0);
////  

////  
////  


////////////////////******************************************************************************************************
////////////////  // Planning with Path Constraints
////////////////////******************************************************************************************************
////////////////  //
////////////////  // Path constraints can easily be specified for a link on the robot.
////////////////  // Let's specify a path constraint and a pose goal for our group.
////////////////  // First define the path constraint.
////////////////  
//////////////////   arm_group.setPlannerId("RRTConnectkConfigDefault");
//////////////////    arm_group.setPlanningTime(45);
////////////////    
////////////////  moveit_msgs::OrientationConstraint ocm;  
////////////////  ocm.link_name = "link_3";  
////////////////  ocm.header.frame_id = "base_link";
////////////////  ocm.orientation.w = 0.50; 
////////////////  ocm.absolute_x_axis_tolerance = 3.14;
////////////////  ocm.absolute_y_axis_tolerance = 3.14;
////////////////  ocm.absolute_z_axis_tolerance = 3.14;
////////////////  ocm.weight = 1.0;
////////////////  
////////////////  // Now, set it as the path constraint for the group.
////////////////  moveit_msgs::Constraints test_constraints;
////////////////  test_constraints.orientation_constraints.push_back(ocm);  
////////////////  group.setPathConstraints(test_constraints);

////////////////  // We will reuse the old goal that we had and plan to it.
////////////////  // Note that this will only work if the current state already 
////////////////  // satisfies the path constraints. So, we need to set the start
////////////////  // state to a new pose. 
////////////////  robot_state::RobotState start_state(*group.getCurrentState());
////////////////  geometry_msgs::Pose start_pose2;
////////////////  
////////////////  start_pose2.position.x = -0.05;
////////////////  start_pose2.position.y = -0.58;
////////////////  start_pose2.position.z = 0.67;
////////////////  
////////////////  start_pose2.orientation.w = 0.20;
////////////////  
////////////////  const robot_state::JointModelGroup *joint_model_group =
////////////////                  start_state.getJointModelGroup(group.getName());
////////////////  start_state.setFromIK(joint_model_group, start_pose2);
////////////////  group.setStartState(start_state);
////////////////  
////////////////  // Now we will plan to the earlier pose target from the new 
////////////////  // start state that we have just created.
////////////////  group.setPoseTarget(target_pose1);
////////////////  success = group.plan(my_plan);

////////////////  ROS_INFO("Visualizing plan 3 (constraints) %s",success?"":"FAILED");
////////////////    ROS_INFO("sleeeeeeeeeeeeeeeeeeeeeeeeeeeeeep");
////////////////  /* Sleep to give Rviz time to visualize the plan. */
////////////////  sleep(15.0);

////////////////  // When done with the path constraint be sure to clear it.
////////////////  group.clearPathConstraints();



//////******************************************************************************************************
////  // Cartesian Paths
//////******************************************************************************************************
////  // You can plan a cartesian path directly by specifying a list of waypoints 
////  // for the end-effector to go through. Note that we are starting 
////  // from the new start state above.  The initial pose (start state) does not
////  // need to be added to the waypoint list.
////  std::vector<geometry_msgs::Pose> waypoints;

////  geometry_msgs::Pose target_pose3 = target_pose1;
////  target_pose3.position.x += 0.1;
////  target_pose3.position.z += 0.05;
////  waypoints.push_back(target_pose3);  // up and out

////  target_pose3.position.z -= 0.05;
////  waypoints.push_back(target_pose3);  // left

////  target_pose3.position.z -= 0.1;
//////  target_pose3.position.y += 0.1;
////  target_pose3.position.x -= 0.05;
////  waypoints.push_back(target_pose3);  // down and right (back to start)

////	

////  // We want the cartesian path to be interpolated at a resolution of 1 cm
////  // which is why we will specify 0.01 as the max step in cartesian
////  // translation.  We will specify the jump threshold as 0.0, effectively
////  // disabling it.
////  moveit_msgs::RobotTrajectory trajectory;
////  double fraction = group.computeCartesianPath(waypoints,
////                                               0.01,  // eef_step
////                                               0.0,   // jump_threshold
////                                               trajectory);

////  ROS_INFO("Visualizing plan 4 (cartesian path) (%.2f%% acheived)",
////        fraction * 100.0);    

////moveit::planning_interface::MoveGroup::Plan plan;
////plan.trajectory_ = trajectory;
////group.execute(plan);
////  
////  ROS_INFO("aqui aqui aqui aqui aqui aqui aqui aqui");  
////      







//	group.setPlannerId("SBLkConfigDefault");
//	group.setPlanningTime(45);
//  // Adding/Removing Objects and Attaching/Detaching Objects
//  // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//  // First, we will define the collision object message.
//  moveit_msgs::CollisionObject collision_object;
//  collision_object.header.frame_id = group.getPlanningFrame();
//	
//	double max_velocity_scaling_factor = 1;
//	group.setMaxVelocityScalingFactor(max_velocity_scaling_factor);

//  /* The id of the object is used to identify it. */
//  collision_object.id = "box1";

//  /* Define a box to add to the world. */
//  shape_msgs::SolidPrimitive primitive;
//  primitive.type = primitive.BOX;
//  primitive.dimensions.resize(3);
//  primitive.dimensions[0] = 0.2;
//  primitive.dimensions[1] = 0.1;
//  primitive.dimensions[2] = 0.7;

//  /* A pose for the box (specified relative to frame_id) */
//  geometry_msgs::Pose box_pose;
//  box_pose.orientation.w = 0.0;
//  box_pose.position.x =  0.35;
//  box_pose.position.y = -0.3;
//  box_pose.position.z =  0.40;

//  collision_object.primitives.push_back(primitive);
//  collision_object.primitive_poses.push_back(box_pose);
//  collision_object.operation = collision_object.ADD;

//  std::vector<moveit_msgs::CollisionObject> collision_objects;  
//  collision_objects.push_back(collision_object);  

//  // Now, let's add the collision object into the world
//  ROS_INFO("Add an object into the world");  
//  planning_scene_interface.addCollisionObjects(collision_objects);
//  
//  /* Sleep so we have time to see the object in RViz */
//  sleep(2.0);

//  // Planning with collision detection can be slow.  Lets set the planning time
//  // to be sure the planner has enough time to plan around the box.  10 seconds
//  // should be plenty.
//  group.setPlanningTime(10.0);


//  // Now when we plan a trajectory it will avoid the obstacle
//  group.setStartState(*group.getCurrentState());
//  group.setPoseTarget(target_pose1);
//  success = group.plan(my_plan);

////	group.execute(my_plan);

//  ROS_INFO("Visualizing plan 5 (pose goal move around box) %s",
//    success?"":"FAILED");
//  /* Sleep to give Rviz time to visualize the plan. */
//  sleep(10.0);
  
  
  
  
  

  // Now, let's attach the collision object to the robot.
//  ROS_INFO("Attach the object to the robot");  
//  group.attachObject(collision_object.id);  
//  /* Sleep to give Rviz time to show the object attached (different color). */
//  sleep(4.0);


  // Now, let's detach the collision object from the robot.
//  ROS_INFO("Detach the object from the robot");  
//  group.detachObject(collision_object.id);  
//  /* Sleep to give Rviz time to show the object detached. */
//  sleep(4.0);


//  // Now, let's remove the collision object from the world.
//  ROS_INFO("Remove the object from the world");  
//  std::vector<std::string> object_ids;
//  object_ids.push_back(collision_object.id);  
//  planning_scene_interface.removeCollisionObjects(object_ids);
//  /* Sleep to give Rviz time to show the object is no longer there. */
//  sleep(4.0);



//  ros::shutdown();  
  return 0;
}

#endif





















































#if 0   //planning_scene

// BEGIN_SUB_TUTORIAL userCallback
//
// User defined constraints can also be specified to the PlanningScene
// class. This is done by specifying a callback using the
// setStateFeasibilityPredicate function. Here's a simple example of a
// user-defined callback that checks whether the "r_shoulder_pan" of
// the PR2 robot is at a positive or negative angle:
bool userCallback(const robot_state::RobotState &kinematic_state, bool verbose)
{
  const double* joint_values = kinematic_state.getJointPositions("joint_6");
  return (joint_values[0] > 0.0);
}



int main(int argc, char **argv)
{
  ros::init (argc, argv, "right_arm_kinematics");
  ros::AsyncSpinner spinner(1);
  spinner.start();

// ^^^^^
// 
// The :planning_scene:`PlanningScene` class can be easily setup and
// configured using a :moveit_core:`RobotModel` or a URDF and
// SRDF. This is, however, not the recommended way to instantiate a
// PlanningScene. The :planning_scene_monitor:`PlanningSceneMonitor`
// is the recommended method to create and maintain the current
// planning scene (and is discussed in detail in the next tutorial)
// using data from the robot's joints and the sensors on the robot. In
// this tutorial, we will instantiate a PlanningScene class directly,
// but this method of instantiation is only intended for illustration.

  robot_model_loader::RobotModelLoader robot_model_loader("robot_description");
  robot_model::RobotModelPtr kinematic_model = robot_model_loader.getModel();
  planning_scene::PlanningScene planning_scene(kinematic_model);

// Collision Checking
////******************************************************************************************************
////******************************************************************************************************
//



////******************************************************************************************************
// Self-collision checking
////******************************************************************************************************
//
// The first thing we will do is check whether the robot in its
// current state is in *self-collision*, i.e. whether the current
// configuration of the robot would result in the robot's parts
// hitting each other. To do this, we will construct a
// :collision_detection_struct:`CollisionRequest` object and a
// :collision_detection_struct:`CollisionResult` object and pass them
// into the collision checking function. Note that the result of
// whether the robot is in self-collision or not is contained within
// the result. Self collision checking uses an *unpadded* version of
// the robot, i.e. it directly uses the collision meshes provided in
// the URDF with no extra padding added on.

  collision_detection::CollisionRequest collision_request;
  collision_detection::CollisionResult collision_result;
  planning_scene.checkSelfCollision(collision_request, collision_result);
  ROS_INFO_STREAM("Test 1: Current state is "
                  << (collision_result.collision ? "in" : "not in")
                  << " self collision");




////******************************************************************************************************
// Change the state
////******************************************************************************************************
//
// Now, let's change the current state of the robot. The planning
// scene maintains the current state internally. We can get a
// reference to it and change it and then check for collisions for the
// new robot configuration. Note in particular that we need to clear
// the collision_result before making a new collision checking
// request.

  robot_state::RobotState& current_state = planning_scene.getCurrentStateNonConst();
  current_state.setToRandomPositions();
  collision_result.clear();
  planning_scene.checkSelfCollision(collision_request, collision_result);
  ROS_INFO_STREAM("Test 2: Current state is "
                  << (collision_result.collision ? "in" : "not in")
                  << " self collision");




////******************************************************************************************************
// Checking for a group
////******************************************************************************************************
//
// Now, we will do collision checking only for the right_arm of the
// PR2, i.e. we will check whether there are any collisions between
// the right arm and other parts of the body of the robot. We can ask
// for this specifically by adding the group name "right_arm" to the
// collision request.

  collision_request.group_name = "manipulator";
  current_state.setToRandomPositions();
  collision_result.clear();
  planning_scene.checkSelfCollision(collision_request, collision_result);
  ROS_INFO_STREAM("Test 3: Current state is "
                  << (collision_result.collision ? "in" : "not in")
                  << " self collision");







////******************************************************************************************************
// Getting Contact Information
////******************************************************************************************************
//
// First, manually set the right arm to a position where we know
// internal (self) collisions do happen. Note that this state is now
// actually outside the joint limits of the PR2, which we can also
// check for directly.

  std::vector<double> joint_values;
  const robot_model::JointModelGroup* joint_model_group =
    current_state.getJointModelGroup("manipulator");
  current_state.copyJointGroupPositions(joint_model_group, joint_values);
  joint_values[0] = 1.57; //hard-coded since we know collisions will happen here
  current_state.setJointGroupPositions(joint_model_group, joint_values);
  ROS_INFO_STREAM("Current state is "
                  << (current_state.satisfiesBounds(joint_model_group) ? "valid" : "not valid"));

// Now, we can get contact information for any collisions that might
// have happened at a given configuration of the right arm. We can ask
// for contact information by filling in the appropriate field in the
// collision request and specifying the maximum number of contacts to
// be returned as a large number.

  collision_request.contacts = true;
  collision_request.max_contacts = 1000;

//

  collision_result.clear();
  planning_scene.checkSelfCollision(collision_request, collision_result);
  ROS_INFO_STREAM("Test 4: Current state is "
                  << (collision_result.collision ? "in" : "not in")
                  << " self collision");
  collision_detection::CollisionResult::ContactMap::const_iterator it;
  for(it = collision_result.contacts.begin();
      it != collision_result.contacts.end();
      ++it)
  {
    ROS_INFO("Contact between: %s and %s",
             it->first.first.c_str(),
             it->first.second.c_str());
  }








////******************************************************************************************************
// Modifying the Allowed Collision Matrix
////******************************************************************************************************
//
// The :collision_detection_class:`AllowedCollisionMatrix` (ACM)
// provides a mechanism to tell the collision world to ignore
// collisions between certain object: both parts of the robot and
// objects in the world. We can tell the collision checker to ignore
// all collisions between the links reported above, i.e. even though
// the links are actually in collision, the collision checker will
// ignore those collisions and return not in collision for this
// particular state of the robot.
//
// Note also in this example how we are making copies of both the
// allowed collision matrix and the current state and passing them in
// to the collision checking function.

  collision_detection::AllowedCollisionMatrix acm = planning_scene.getAllowedCollisionMatrix();
  robot_state::RobotState copied_state = planning_scene.getCurrentState();

  collision_detection::CollisionResult::ContactMap::const_iterator it2;
  for(it2 = collision_result.contacts.begin();
      it2 != collision_result.contacts.end();
      ++it2)
  {
    acm.setEntry(it2->first.first, it2->first.second, true);
  }
  collision_result.clear();
  planning_scene.checkSelfCollision(collision_request, collision_result, copied_state, acm);
  ROS_INFO_STREAM("Test 5: Current state is "
                  << (collision_result.collision ? "in" : "not in")
                  << " self collision");












////******************************************************************************************************
  // Full Collision Checking
////******************************************************************************************************
  //
  // While we have been checking for self-collisions, we can use the
  // checkCollision functions instead which will check for both
  // self-collisions and for collisions with the environment (which is
  // currently empty).  This is the set of collision checking
  // functions that you will use most often in a planner. Note that
  // collision checks with the environment will use the padded version
  // of the robot. Padding helps in keeping the robot further away
  // from obstacles in the environment.*/
  collision_result.clear();
  planning_scene.checkCollision(collision_request, collision_result, copied_state, acm);
  ROS_INFO_STREAM("Test 6: Current state is "
                  << (collision_result.collision ? "in" : "not in")
                  << " self collision");






  // Constraint Checking
////******************************************************************************************************
////******************************************************************************************************
  //
  // The PlanningScene class also includes easy to use function calls
  // for checking constraints. The constraints can be of two types:
  // (a) constraints chosen from the
  // :kinematic_constraints:`KinematicConstraint` set:
  // i.e. :kinematic_constraints:`JointConstraint`,
  // :kinematic_constraints:`PositionConstraint`,
  // :kinematic_constraints:`OrientationConstraint` and
  // :kinematic_constraints:`VisibilityConstraint` and (b) user
  // defined constraints specified through a callback. We will first
  // look at an example with a simple KinematicConstraint.
  //
  
////******************************************************************************************************
  // Checking Kinematic Constraints
////******************************************************************************************************
  //
  // We will first define a simple position and orientation constraint
  // on the end-effector of the right_arm of the PR2 robot. Note the
  // use of convenience functions for filling up the constraints
  // (these functions are found in the :moveit_core_files:`utils.h<utils_8h>` file from the
  // kinematic_constraints directory in moveit_core).

  std::string end_effector_name = joint_model_group->getLinkModelNames().back();

  geometry_msgs::PoseStamped desired_pose;
  desired_pose.pose.orientation.w = -0.042837;
  desired_pose.pose.position.x = -0.10;
  desired_pose.pose.position.y = -0.58;
  desired_pose.pose.position.z = 0.67;
  desired_pose.header.frame_id = "base_link";
  moveit_msgs::Constraints goal_constraint =
    kinematic_constraints::constructGoalConstraints(end_effector_name, desired_pose);

// Now, we can check a state against this constraint using the
// isStateConstrained functions in the PlanningScene class.

  copied_state.setToRandomPositions();
  copied_state.update();
  bool constrained = planning_scene.isStateConstrained(copied_state, goal_constraint);
  ROS_INFO_STREAM("Test 7: Random state is "
                  << (constrained ? "constrained" : "not constrained"));

// There's a more efficient way of checking constraints (when you want
// to check the same constraint over and over again, e.g. inside a
// planner). We first construct a KinematicConstraintSet which
// pre-processes the ROS Constraints messages and sets it up for quick
// processing.

  kinematic_constraints::KinematicConstraintSet kinematic_constraint_set(kinematic_model);
  kinematic_constraint_set.add(goal_constraint, planning_scene.getTransforms());
  bool constrained_2 =
    planning_scene.isStateConstrained(copied_state, kinematic_constraint_set);
  ROS_INFO_STREAM("Test 8: Random state is "
                  << (constrained_2 ? "constrained" : "not constrained"));

// There's a direct way to do this using the KinematicConstraintSet
// class.

  kinematic_constraints::ConstraintEvaluationResult constraint_eval_result =
    kinematic_constraint_set.decide(copied_state);
  ROS_INFO_STREAM("Test 9: Random state is "
                  << (constraint_eval_result.satisfied ? "constrained" : "not constrained"));





////******************************************************************************************************
// User-defined constraints
////******************************************************************************************************
//
// CALL_SUB_TUTORIAL userCallback

// Now, whenever isStateFeasible is called, this user-defined callback
// will be called.

  planning_scene.setStateFeasibilityPredicate(userCallback);
  bool state_feasible = planning_scene.isStateFeasible(copied_state);
  ROS_INFO_STREAM("Test 10: Random state is "
                  << (state_feasible ? "feasible" : "not feasible"));

// Whenever isStateValid is called, three checks are conducted: (a)
// collision checking (b) constraint checking and (c) feasibility
// checking using the user-defined callback.

  bool state_valid =
    planning_scene.isStateValid(copied_state, kinematic_constraint_set, "manipulator");
  ROS_INFO_STREAM("Test 10: Random state is "
                  << (state_valid ? "valid" : "not valid"));

// Note that all the planners available through MoveIt! and OMPL will
// currently perform collision checking, constraint checking and
// feasibility checking using user-defined callbacks.
// END_TUTORIAL

  ros::shutdown();
  return 0;
}

#endif





















































#if 0   //planning_scene

int main(int argc, char **argv)
{
  ros::init (argc, argv, "right_arm_kinematics");
  ros::AsyncSpinner spinner(1);
  spinner.start();

  ros::NodeHandle node_handle;

  // Start a service client
  ros::ServiceClient service_client = node_handle.serviceClient<moveit_msgs::GetPositionIK> ("compute_ik");
  ros::Publisher robot_state_publisher = node_handle.advertise<moveit_msgs::DisplayRobotState>( "robot_state", 1 );

  while(!service_client.exists())
  {
    ROS_INFO("Waiting for service");
    sleep(1.0);
  }

  moveit_msgs::GetPositionIK::Request service_request;
  moveit_msgs::GetPositionIK::Response service_response;

  service_request.ik_request.group_name = "manipulator";
  service_request.ik_request.pose_stamped.header.frame_id = "tool0";
  service_request.ik_request.pose_stamped.pose.position.x = -0.10;
  service_request.ik_request.pose_stamped.pose.position.y = -0.58;
  service_request.ik_request.pose_stamped.pose.position.z = 0.67;

  service_request.ik_request.pose_stamped.pose.orientation.x = -0.9;
  service_request.ik_request.pose_stamped.pose.orientation.y = -0.964875;
  service_request.ik_request.pose_stamped.pose.orientation.z = 0.256041;
  service_request.ik_request.pose_stamped.pose.orientation.w = -0.0428377;

  /* Call the service */
  service_client.call(service_request, service_response);
  ROS_INFO_STREAM("Result: " << ((service_response.error_code.val == service_response.error_code.SUCCESS) ? "True " : "False ") << service_response.error_code.val);

  /* Filling in a seed state */
  robot_model_loader::RobotModelLoader robot_model_loader("robot_description");
  robot_model::RobotModelPtr kinematic_model = robot_model_loader.getModel();
  robot_state::RobotStatePtr kinematic_state(new robot_state::RobotState(kinematic_model));
  const robot_state::JointModelGroup* joint_model_group = kinematic_model->getJointModelGroup("manipulator");

  /* Get the names of the joints in the right_arm*/
  service_request.ik_request.robot_state.joint_state.name = joint_model_group->getJointModelNames();

  /* Get the joint values and put them into the message, this is where you could put in your own set of values as well.*/
  kinematic_state->setToRandomPositions(joint_model_group);
  kinematic_state->copyJointGroupPositions(joint_model_group, service_request.ik_request.robot_state.joint_state.position);

  /* Call the service again*/
  service_client.call(service_request, service_response);
  ROS_INFO_STREAM("Result: " << ((service_response.error_code.val == service_response.error_code.SUCCESS) ? "True " : "False ") << service_response.error_code.val);

  /* Check for collisions*/
  service_request.ik_request.avoid_collisions = true;

  /* Call the service again*/
  service_client.call(service_request, service_response);

  ROS_INFO_STREAM("Result: " << ((service_response.error_code.val == service_response.error_code.SUCCESS) ? "True " : "False ") << service_response.error_code.val);

  /* Visualize the result*/
  moveit_msgs::DisplayRobotState msg;
  kinematic_state->setVariableValues(service_response.solution.joint_state);
  robot_state::robotStateToRobotStateMsg(*kinematic_state, msg.state);
  robot_state_publisher.publish(msg);

  //Sleep to let the message go through
  ros::Duration(2.0).sleep();

  ros::shutdown();
  return 0;
}

#endif
