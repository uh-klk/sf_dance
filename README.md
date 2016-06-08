# sf_dance
Required sf_aria_driver

To lunch sf_dance:
    roslaunch sf_dance sf_dance.launch

To test sf_dance:
    rostopic pub -1 /sf_dance/sf_dance_filename std_msgs/String "Happy.xml"
