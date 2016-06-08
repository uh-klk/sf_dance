// - xml folder should be in the same directory as the generated exefile
//rostopic pub /sf_dance/sf_dance_filename  std_msgs/Strin Excited.xml

#include "ros/ros.h"
#include "std_msgs/String.h"  //for receiving filename 

#include "sf_dance/DanceStep.h"
#include "sf_dance/DanceSequence.h"

#include "dance_parser.hpp"

#include <string.h>

using namespace std;

class SFDance
{
public:

    SFDance(string xml_path, ros::NodeHandle nh) : xml_path_(xml_path), node_(nh)
    {
        //TODO: need create a param for changing the path

        dance_parser_ = new DanceParser(xml_path_);
    }

    ~SFDance(){}

    void init();

    void receiveCB(const std_msgs::String::ConstPtr& msg);

    void retrieveDanceSequences(std::vector<Behaviour> *dance_sequences,
                                         string xml_filename)
    {
        printf("In sf_dance:retrieveDanceSequences");
        dance_parser_->retrieveDanceSequences(dance_sequences, xml_filename);
    }

    void sendDanceSequences(std::vector<Behaviour> *dance_sequences);

protected:
    ros::NodeHandle node_;
    ros::Subscriber subscriber_;
    ros::Publisher publisher_;
    string xml_path_;
    DanceParser *dance_parser_;
};

void SFDance::init()
{
    // xml filename of the dance sequences to be performed
    subscriber_ = node_.subscribe("sf_dance_filename", 1000, &SFDance::receiveCB, this);

    // send parsed dance sequences msg to robot base
    publisher_ = node_.advertise<sf_dance::DanceSequence>("/RosAria/aria_dance_sequence", 1000);
}

void SFDance::receiveCB(const std_msgs::String::ConstPtr& msg)
{
    string xml_filename;
    std::vector<Behaviour> dance_sequences;

    xml_filename = msg->data;

    ROS_INFO("I heard mode: [%s]", msg->data.c_str()); //filename

    retrieveDanceSequences(&dance_sequences, xml_filename);

    sendDanceSequences(&dance_sequences);
}

void SFDance::sendDanceSequences(std::vector<Behaviour> *dance_sequences)
{

    sf_dance::DanceSequence msg;

    sf_dance::DanceStep danceStep;

    //not yet implemented in the xml file hence we set it to default here
    msg.rotAccel = sf_dance::DanceSequence::DEFAULT;
    msg.rotDecel = sf_dance::DanceSequence::DEFAULT;
    msg.transAccel = sf_dance::DanceSequence::DEFAULT;
    msg.transDecel = sf_dance::DanceSequence::DEFAULT;

    for(std::vector<Behaviour>::const_iterator it=dance_sequences->begin(); it !=dance_sequences->end(); ++it)
    {
        //user for idenfiying the behaviour and the default preference
        //it->id;
        //it->type;
        //it->userPreference; //default user preference
        for(std::vector<Channel>::const_iterator it1= it->channel.begin(); it1 !=it->channel.end(); ++it1)
        {   // modifier for use with user's preference
            //=it1->m_value1;
            //=it1->m_value2;
            //=it1->m_value3;
            for(std::vector<Sequence>::const_iterator it2= it1->sequence.begin(); it2 !=it1->sequence.end(); ++it2)
            {
                for(std::vector<Step>::const_iterator it3= it2->step.begin(); it3 !=it2->step.end(); ++it3)
                {
                    danceStep.mode = it3->mode;
                    danceStep.value = it3->value;
                    msg.danceStep.push_back(danceStep);
                }
            }

        }
    }

    publisher_.publish(msg);
}

int main(int argc, char* argv[])
{
    string danceXML_path = "behaviourXML/dance/"; //default path

    ros::init(argc, argv, "sf_dance");
    ros::NodeHandle n(std::string("~"));
    
    n.getParam("danceXML_path",danceXML_path);    //load the param from launch file into the variable

    ROS_INFO("%s", danceXML_path.c_str());

    SFDance *sf_dance = new SFDance(danceXML_path, n);
    sf_dance->init(); //init subscriber and publisher

    ros::Rate loop_rate(100);

    while (ros::ok())
    {
        ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;
}

