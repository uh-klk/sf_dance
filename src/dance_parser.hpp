//TuneParcer: Reads tune from a XML file, interpretes and returns
// a sequence of tones at a time taking into account of the sequence's
// timing
// Todo: - taking user preferences into account (preference from topic to compile the tune)
//       - better name for TuneTargetState

#ifndef DANCE_PARSER_HPP_INCLUDED
#define DANCE_PARSER_HPP_INCLUDED

#include <iostream>
#include <vector>
#include <fstream>
#include <string.h>

#include "rapidxml/rapidxml.hpp"

using namespace rapidxml;
using namespace std;

struct Step
{
    int mode;
    int value;
};

struct Sequence
{
    std::vector<Step> step;
};

struct Channel
{
    int m_value1;
    int m_value2;
    int m_value3;
    std::vector<Sequence> sequence;
};

struct Behaviour
{
    string id;
    string type;
    string userPreference;
    std::vector<Channel> channel;
};


class DanceParser {

public:
    DanceParser(string xmlPath);
    ~DanceParser(){}

    void parseDefaultPathXML(std::vector<Behaviour> *behaviourExport, string xml_filename); //xml in default folder
    void parseFullPathXML(std::vector<Behaviour> *behaviourExport, string xml_fullpath_filename); //full path to xml
    void retrieveDanceSequences(std::vector<Behaviour> *behaviourExport, string xml_filename)
    {
        parseDefaultPathXML(behaviourExport, xml_filename); //initiliased the request behaviour
    }

private:
    string default_xml_path_;
};

DanceParser::DanceParser(string default_xml_path) :  default_xml_path_(default_xml_path)
{
    printf("\nIn DefaultPathXML before %s", default_xml_path.c_str());
}

void DanceParser::parseDefaultPathXML(std::vector<Behaviour> *behaviourExport, string xml_filename)
{
  string xml_fullpath_filename;

  xml_fullpath_filename  = default_xml_path_; //stored the path
  xml_fullpath_filename += xml_filename;    // add the file name

  parseFullPathXML(behaviourExport, xml_fullpath_filename);     //import tune from the XML file
}

void DanceParser::parseFullPathXML(std::vector<Behaviour> * behaviourExport, string xml_fullpath_filename)
{

    //std::vector<Behaviour> behaviourExport;
    Behaviour behaviour;
    Channel channel;
    Sequence sequence;
    Step step;

    ifstream *ifs;
    string line;
    string buf;
    char *doc;

    // Behaviour attributes
    string id, type, userPreference;
    id = "id"; type = "type"; userPreference = "userPreference";

    // Channel attributes
    string m_value1, m_value2, m_value3;
    m_value1 = "m_value1";  m_value2 = "m_value2";  m_value3 = "m_value3";

    // Step attributes
    string mode, value;
    mode = "mode";  value = "value";


    ifs = new ifstream(xml_fullpath_filename.c_str());
 
    if( !ifs->good() )
    {
        printf("Invalid filename: %s\n", xml_fullpath_filename.c_str());
        exit(1);
    }
  
    while( ifs->good() )
    {
        while(getline(*ifs, line))  //copy xml contents into buf
          buf += line;

        doc = new char[buf.length()+1];
        strcpy(doc, buf.c_str());

        xml_document<> behaviour_xml; // character type defaults to char
        behaviour_xml.parse<0>(doc);  // 0 means default parse flags

        behaviourExport->clear();
        xml_node<> *behaviourNode = behaviour_xml.first_node();
        //Behaviour attribute
        for( xml_attribute<> *behaviourAttribute = behaviourNode->first_attribute();
             behaviourAttribute; behaviourAttribute = behaviourAttribute->next_attribute())
        {
            if( !id.compare(behaviourAttribute->name()) )
                behaviour.id = behaviourAttribute->value();

            else if( !type.compare(behaviourAttribute->name()) )
                behaviour.type = behaviourAttribute->value();

            else if( !userPreference.compare(behaviourAttribute->name()) )
                behaviour.userPreference = atoi(behaviourAttribute->value());
        }

        behaviour.channel.clear();
        //Channel elements
        for (xml_node<> *channelNode = behaviourNode->first_node();
             channelNode; channelNode = channelNode->next_sibling())
        {
        // Channel attributes
            for (xml_attribute<> *channelAttribute = channelNode->first_attribute();
                 channelAttribute; channelAttribute = channelAttribute->next_attribute())
            {
                if( !m_value1.compare(channelAttribute->name()) )
                    channel.m_value1 = atoi(channelAttribute->value());

                else if( !m_value2.compare(channelAttribute->name()) )
                    channel.m_value2 = atoi(channelAttribute->value());

                else if( !m_value3.compare(channelAttribute->name()) )
                    channel.m_value3 = atoi(channelAttribute->value());
            }

            // Sequence elements
            channel.sequence.clear();
            for( xml_node<> *sequenceNode = channelNode->first_node();
                 sequenceNode; sequenceNode = sequenceNode->next_sibling())  //each sequence
            {
                // Step elements
                sequence.step.clear();
                for(xml_node<> *stepNode = sequenceNode->first_node();
                    stepNode; stepNode = stepNode->next_sibling())  //each step
                {
                    // Step attributes
                    for(xml_attribute<> *stepAttribute = stepNode->first_attribute();
                        stepAttribute; stepAttribute = stepAttribute->next_attribute()) //each attributes
                    {
                        if( !mode.compare(stepAttribute->name()) )
                            step.mode = atoi(stepAttribute->value());

                        else if( !value.compare(stepAttribute->name()) )
                            step.value = atoi(stepAttribute->value());
                    }
                    sequence.step.push_back(step);
                }
                channel.sequence.push_back(sequence);
            }
            //behaviour contains all the data
            behaviour.channel.push_back(channel);
        }
        behaviourExport->push_back(behaviour);
    }
    ifs->close();

    /*
    cout<<"Debuging: For checking the output";
    for(std::vector<Behaviour>::const_iterator it=behaviourExport->begin(); it !=behaviourExport->end(); ++it)
        for(std::vector<Channel>::const_iterator it1= it->channel.begin(); it1 !=it->channel.end(); ++it1)
            for(std::vector<Sequence>::const_iterator it2= it1->sequence.begin(); it2 !=it1->sequence.end(); ++it2)
                for(std::vector<Step>::const_iterator it3= it2->step.begin(); it3 !=it2->step.end(); ++it3)
                    cout<<' '<<it3->value;


    */

}

#endif
