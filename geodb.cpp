//
//  geodb.cpp
//  project4
//
//  Created by Larry Lim on 3/7/24.
//

#include "geodb.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include <string>
#include <vector>
#include <functional>
#include "geotools.h"


GeoDatabase::GeoDatabase(){
    
}

GeoDatabase::~GeoDatabase(){
    
}

bool GeoDatabase::load(const std::string& map_data_file){
    
    std::ifstream is(map_data_file);
    
    if (!is) {
        return false;
    }
    
    std::string line;
    while(std::getline(is,line)){           //get each line
        std::string name = line;            //set name to first line
        
        std::string x1, y1, x2, y2;                 //get 4 cordinates
        if(!(is >> x1 >> y1 >> x2 >> y2)){
            return false;
        }
        GeoPoint start(x1, y1);                     //geo point start coord and end coord
        GeoPoint end(x2, y2);


        connectedStreetMap.insert(start.to_string() + end.to_string(), name);        //set streetname for S1 and S2
        connectedStreetMap.insert(end.to_string() + start.to_string(), name);

      
        
        if(connectedPointsMap.find(start.to_string())){         //connecting points, checking if a point already associates
            std::vector<GeoPoint> newvalue = connectedPointsMap[start.to_string()]; //appending connections to a vector
            newvalue.push_back(end);
            connectedPointsMap.insert(start.to_string(), newvalue);
        } else {
            connectedPointsMap.insert(start.to_string(), {end});        //start -> end
        }
        if(connectedPointsMap.find(end.to_string())){
            std::vector<GeoPoint> newvalue = connectedPointsMap[end.to_string()];
            newvalue.push_back(start);
            connectedPointsMap.insert(end.to_string(), newvalue);
        } else {
            connectedPointsMap.insert(end.to_string(), {start}); //end -> start
        }
        
        int poinum;                     //# of POIs
        if(!(is >> poinum)){
            return false;
        }
        is.ignore(1000,'\n');
        if(poinum > 0){                             //if POI is greater than 0, Step #2
            GeoPoint mid = midpoint(start, end);
            connectedPointsMap.insert(mid.to_string(), {start, end});               //connect midpoint to S1 and S2
           
            std::vector<GeoPoint> newvalue = connectedPointsMap[start.to_string()]; //S1 -> mid
            newvalue.push_back(mid);
            connectedPointsMap.insert(start.to_string(), newvalue);
            
            std::vector<GeoPoint> newvalue2 = connectedPointsMap[end.to_string()];  //S2 -> mid
            newvalue2.push_back(mid);
            connectedPointsMap.insert(end.to_string(), newvalue2);
            
       

            connectedStreetMap.insert(mid.to_string() + start.to_string(), name);         //connect midpoint wiht s1 and s2
            connectedStreetMap.insert(mid.to_string() + end.to_string(), name);
            connectedStreetMap.insert(start.to_string() + mid.to_string(), name);                 //connect s1 and s2 with midpoint
            connectedStreetMap.insert(end.to_string() + mid.to_string(), name);
            
            for(int i = 0; i < poinum; i++){    //terate through each line for POI times
                if(!getline(is, line)){
                    return false;
                }
                std::istringstream poi_line_stream(line);
                std::string poi_name;
                std::string poi_coords;                 //parse string, name | coords, store invariables
                
                if (std::getline(poi_line_stream, poi_name, '|') && std::getline(poi_line_stream, poi_coords)) {
                    std::istringstream coords_stream(poi_coords);
                    std::string poicord1, poicord2;
                    if (!(coords_stream >> poicord1 >> poicord2)) {    //split the two cords into x and y
                        return false;
                    }
                    GeoPoint poicord(poicord1, poicord2);
                    poiLocationsMap.insert(poi_name, poicord);          //have poi name (key) map to cord (value)
                    
                    connectedPointsMap.insert(poicord.to_string(), {mid});                  //connect midpoint with POIS
                    std::vector<GeoPoint> newvalue3 = connectedPointsMap[mid.to_string()];
                    newvalue3.push_back(poicord);
                    connectedPointsMap.insert(mid.to_string(), newvalue3);
                    
                    
                    //POI -> midpoint = a path
                    //POI -> POI  = ""
                    //POI->street segment = ""

                    std::string path = "a path";
                    connectedStreetMap.insert(poicord.to_string() + mid.to_string(), path);   //connect POI with midpoint and viceversa
                    connectedStreetMap.insert(mid.to_string() + poicord.to_string(), path);
                }
            }
        }
    }
    return true;
}


bool GeoDatabase::get_poi_location(const std::string& poi,GeoPoint& point) const{
    const GeoPoint* poi_location = poiLocationsMap.find(poi);   //find POI key in hashmap
    if(poi_location){                                           //if we find it, set parameter to poi cord
        point = *poi_location;
        return true;
    }
    return false;
}


std::vector<GeoPoint> GeoDatabase::get_connected_points(const GeoPoint& pt) const{
    const std::vector<GeoPoint>* connect = connectedPointsMap.find(pt.to_string());     //search through connected points
    if(connect){
        return *connect;                                //return connected points
    }
    else{
        return std::vector<GeoPoint>();                 //no connected points, empty vector
    }
}

std::string GeoDatabase::get_street_name(const GeoPoint& pt1, const GeoPoint& pt2) const {
        const std::string* streetname3 = connectedStreetMap.find(pt1.to_string() + pt2.to_string());
        if(streetname3){
            return (*streetname3);
        }
        return "";
    }
