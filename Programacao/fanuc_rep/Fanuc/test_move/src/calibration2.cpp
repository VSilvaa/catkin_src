#include <ros/ros.h>
#include <moveit/move_group_interface/move_group.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>
#include <moveit_msgs/DisplayRobotState.h>
#include <moveit_msgs/DisplayTrajectory.h>
#include <moveit_msgs/AttachedCollisionObject.h>
#include <moveit_msgs/CollisionObject.h>
#include <iostream>
//###############Includes para o grupo do inverse kinematic
#include <moveit/robot_model_loader/robot_model_loader.h>
#include <moveit/planning_scene/planning_scene.h>
//#include <moveit/joint_model_group.h>
#include <moveit_msgs/AttachedCollisionObject.h>
#include <moveit_msgs/CollisionObject.h>
#include <ros/ros.h>
#include <moveit/move_group_interface/move_group.h>
#include <moveit_msgs/DisplayRobotState.h>
#include <moveit_msgs/DisplayTrajectory.h>
//###############Includes para o grupo do inverse kinematic
#include <moveit/robot_model_loader/robot_model_loader.h>
#include <moveit/planning_scene/planning_scene.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>
//#include <moveit/joint_model_group.h>
#include <moveit/move_group_interface/move_group.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>
#include <moveit_msgs/DisplayRobotState.h>
#include <moveit_msgs/DisplayTrajectory.h>
#include <moveit_msgs/AttachedCollisionObject.h>
#include <moveit_msgs/CollisionObject.h>

#include <boost/function.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/assert.hpp>
#include <math.h>
#include <moveit/trajectory_processing/iterative_time_parameterization.h>
//#include <moveit_cartesian_plan_plugin/generate_cartesian_path.h>
#include <geometry_msgs/PoseArray.h>

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




#include <industrial_trajectory_filters/filter_base.h>
#include <industrial_trajectory_filters/uniform_sample_filter.h>
#include <moveit/trajectory_processing/iterative_time_parameterization.h>
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
#include <tf/transform_listener.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>


volatile sig_atomic_t stop;
		 Eigen::MatrixXf Final(4,4);
		 Eigen::MatrixXf FinalRobo(4,4);
		 Eigen::MatrixXf FinalMultiplicacao(4,4);

void inthand(int signum) {
	std::cout << "\ncuidado que eu sai.........................................................................................................................\n";
//	FinalRobo << 1,0,0,0,
//				 0,1,0,0,
//				 0,0,1,0,
//				 0,0,0,1;
	FinalMultiplicacao=Final.inverse();
	std::cout << "\nFinalMultiplicacao==\n" << FinalMultiplicacao;
	
    exit(1); 
}


class cloudHandler
{
public:
    cloudHandler()
    {
    	subscriber = node_handle.subscribe("/camera/rgb/aruco_tracker/transform", 1000, &cloudHandler::positionCallback, this);
//    	subscriber_robo = node_handle.subscribe("/tf_static", 1000, &cloudHandler::positionRoboCallback, this);
//    	subscriber_robo1 = node_handle.subscribe("/tf", 1000, &cloudHandler::positionRoboCallback, this);
    }

	void positionCallback(const geometry_msgs::TransformStamped & msgTf)
	{
//		ROS_INFO("\n \n NOVO NOVO NOVO NOVO NOVO NOVO NOVO!!!! \n");
//	 	ROS_INFO("Sequence ID:[%d]",msgTf.header.seq);
//		ROS_INFO("msg time:[%d,%d]",msgTf.header.stamp.sec,msgTf.header.stamp.nsec);
//		ROS_INFO("tf[x=%f,y=%f,z=%f]",msgTf.transform.translation.x,msgTf.transform.translation.y,msgTf.transform.translation.z);
//		ROS_INFO("orientation[x=%f,y=%f,z=%f,w=%f]",msgTf.transform.rotation.x,msgTf.transform.rotation.y,msgTf.transform.rotation.z,msgTf.transform.rotation.w);
		
		float Transx = msgTf.transform.translation.x;
		
//		std::cout << Transx << "terminou \n";
		
	  	
//		ROS_INFO("\n \n POSICAO ATINGIDA!!!!");
	

		tf::Quaternion q(msgTf.transform.rotation.x, msgTf.transform.rotation.y, msgTf.transform.rotation.z, msgTf.transform.rotation.w);
		tf::Matrix3x3 m(q);
		double roll, pitch, yaw;
		m.getRPY(roll, pitch, yaw);

//		std::cout << "\nRoll: " << roll << ", Pitch: " << pitch << ", Yaw: " << yaw << std::endl;

		Eigen::AngleAxisd rollAngle(roll, Eigen::Vector3d::UnitZ());
		Eigen::AngleAxisd yawAngle(yaw, Eigen::Vector3d::UnitY());
		Eigen::AngleAxisd pitchAngle(pitch, Eigen::Vector3d::UnitX());

		Eigen::Quaternion<double> q1 = rollAngle * yawAngle * pitchAngle;

		Eigen::Matrix3d rotationMatrix = q1.matrix();

//		std::cout << "\nMmmmmm==\n" << rotationMatrix;
	
//		 Eigen::MatrixXf Final(4,4);
		 Final << rotationMatrix(0,0), 		rotationMatrix(0,1), 		rotationMatrix(0,2), 		msgTf.transform.translation.x,
		 		  rotationMatrix(1,0),		rotationMatrix(1,1),		rotationMatrix(1,2),		msgTf.transform.translation.y,
		 		  rotationMatrix(2,0),		rotationMatrix(2,1),		rotationMatrix(2,2),		msgTf.transform.translation.z,
		 		  0,						0,							0,							1;
		 
//		 std::cout << "\nFinal==\n" << Final;
		 
		 std::cout << "\nThe inverse of FinalCamera is:\n" << Final.inverse();
	}
	
	
	
	
//	void positionRoboCallback(const tf::tfMessage::ConstPtr& msgTf2)
//	{
////	tf2_msgs/TFMessage
//	tf::TransformListener listener;
////  ros::Rate rate(10.0);
//	volatile sig_atomic_t stop;
//	
//	while (ros::ok()){
//		tf::StampedTransform transform;
//		try{
//			listener.lookupTransform("/base_link", "/tool0",
//			ros::Time(0), transform);
//			
//			
//			ROS_INFO("\n \n AQUI AQUI AQUI AQUI AQUI AQUI AQUI!!!! \n");
//			std::cout << "x==" << transform.getOrigin().x() << "\n";
//			std::cout << "y==" << transform.getOrigin().y() << "\n";
//			std::cout << "z==" << transform.getOrigin().z() << "\n";
//			std::cout << "xx==" << transform.getRotation().x() << "\n";
//			std::cout << "yy==" << transform.getRotation().y() << "\n";
//			std::cout << "zz==" << transform.getRotation().z() << "\n";
//			std::cout << "ww==" << transform.getRotation().w() << "\n";
//			std::cout << "yaw==" << tf::getYaw(transform.getRotation()) << "\n";
//			
//			tf::Quaternion q_robo(transform.getRotation().x(), transform.getRotation().y(), transform.getRotation().z(), transform.getRotation().w());
//			tf::Matrix3x3 m_robo(q_robo);
//			double roll_robo, pitch_robo, yaw_robo;
//			m_robo.getRPY(roll_robo, pitch_robo, yaw_robo);

//			std::cout << "\nRoll: " << roll_robo << ", Pitch: " << pitch_robo << ", Yaw: " << yaw_robo << std::endl;

//			Eigen::AngleAxisd rollAngle(roll_robo, Eigen::Vector3d::UnitZ());
//			Eigen::AngleAxisd yawAngle(yaw_robo, Eigen::Vector3d::UnitY());
//			Eigen::AngleAxisd pitchAngle(pitch_robo, Eigen::Vector3d::UnitX());

//			Eigen::Quaternion<double> q1_robo = rollAngle * yawAngle * pitchAngle;

//			Eigen::Matrix3d rotationMatrixRobo = q1_robo.matrix();

//	//		std::cout << "\nMmmmmm==\n" << rotationMatrix;
//	
////			 Eigen::MatrixXf FinalRobo(4,4);
//			 FinalRobo << rotationMatrixRobo(0,0), 		rotationMatrixRobo(0,1), 		rotationMatrixRobo(0,2), 		transform.getOrigin().x(),
//				 		  rotationMatrixRobo(1,0),		rotationMatrixRobo(1,1),		rotationMatrixRobo(1,2),		transform.getOrigin().y(),
//				 		  rotationMatrixRobo(2,0),		rotationMatrixRobo(2,1),		rotationMatrixRobo(2,2),		transform.getOrigin().z(),
//				 		  0,							0,								0,								1;
//			 
//			 std::cout << "\nFinalRobo==\n" << FinalRobo;

////			ros::Duration(1.5).sleep();
//			sleep(2);

//		}
//		catch (tf::TransformException &ex) {
//		ROS_ERROR("%s",ex.what());
//		sleep(1);
//		continue;
//		}


//		sleep(1);
//  }
////	tf::TransformListener listener;
////	tf::StampedTransform transform;
////			ROS_INFO("\n \n AQUI AQUI AQUI AQUI AQUI AQUI AQUI!!!! \n");
////	listener.waitForTransform("/base_link", "/tool0", ros::Time::now(), ros::Duration(4.0));
////    listener.lookupTransform("/base_link", "/tool0",ros::Time::now(), transform);

////		ROS_INFO("\n \n NOVO NOVO NOVO NOVO NOVO NOVO NOVO!!!! \n");
//		
//		
////	 	ROS_INFO("Sequence ID:[%d]",msgTf2.header.seq);
////		ROS_INFO("msg time:[%d,%d]",msgTf2.header.stamp.sec,msgTf2.header.stamp.nsec);
////		ROS_INFO("tf[x=%f,y=%f,z=%f]",msgTf2.transform.translation.x,msgTf2.transform.translation.y,msgTf2.transform.translation.z);
//	}

protected:
	ros::NodeHandle node_handle;
    ros::Subscriber subscriber;
//    ros::Subscriber subscriber_robo;    
//    ros::Subscriber subscriber_robo1;    
    
//    ros::Publisher pcl_pub, ind_pub, coef_pub;
};





int main(int argc, char **argv)
{

	ros::init(argc, argv, "matrix_CamToXadrez");

	ros::AsyncSpinner spinner(1);
	
    spinner.start();
    
  	signal(SIGINT, inthand);
  	
  	cloudHandler handler;
  
//	ros::Subscriber subscriber = node_handle.subscribe("/camera/rgb/aruco_tracker/transform", 1000, positionCallback);

	std::cout << "passei no fim==================================================================================================================================" << "\n";
		
		ros::spin();
	
	

  return 0;
}
