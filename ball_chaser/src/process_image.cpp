#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    if (!client.call(srv))
        ROS_ERROR("Failed to call service");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    int position = 0;
    int rgb_pixel = -1;
    bool white_ball_found = false;
    
    int rgb_white = 255*3;

    int left_position = 0;
    int left_middle_position = img.width/3;
    int right_middle_position = (img.width/3)*2;
    int right_position = img.width;

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
    for (int row = 0; row < img.height; row++){
        for (int col = 0; col < img.width; col++){
           rgb_pixel = img.data[row*img.step+col*3+0] + img.data[row*img.step+col*3+1] + img.data[row*img.step+col*3+2];
           if(rgb_pixel == rgb_white){
              white_ball_found = true;
              position = col;
           }
        } 
    }

    //ROS_INFO_STREAM("Position  " << position );

    if(white_ball_found == true){
       //Left
       if (position > left_position && position < left_middle_position){
          drive_robot(0, 0.2);
          //ROS_INFO_STREAM("LEFT");
       }

       //Middle
       else if (position >= left_middle_position && position < right_middle_position){
          drive_robot(0.2, 0);
          //ROS_INFO_STREAM("MIDDLE");
       }

       //Right
       else if (position >= right_middle_position && position < right_position){
          drive_robot(0, -0.2);
          //ROS_INFO_STREAM("RIGHT");
       }
    }

    //No White Ball
    else {
        drive_robot(0, 0);
        //ROS_INFO_STREAM("NA");
    }


}

int main(int argc, char **argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
