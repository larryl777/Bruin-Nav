////
////  router.cpp
////  project4
////
////  Created by Larry Lim on 3/9/24.
////
//
#include "router.h"


#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include "geotools.h"
#include <queue>
#include <unordered_set>
#include <iostream>
#include <unordered_map>

bool operator==(const GeoPoint& lhs, const GeoPoint& rhs){
    return lhs.sLatitude == rhs.sLatitude && lhs.sLongitude == rhs.sLongitude;
}

bool operator!=(const GeoPoint& lhs, const GeoPoint& rhs){
    return !(lhs==rhs);
}

Router::Router(const GeoDatabaseBase& geo_db): RouterBase(), database(geo_db){
}

struct PriorityQueueCompare {
    bool operator()(const std::pair<GeoPoint, double>& a, const std::pair<GeoPoint, double>& b) const {
        return a.second > b.second;                         // smaller distance has higher priority
    }
};

std::vector<GeoPoint> Router::route(const GeoPoint& pt1, const GeoPoint& pt2) const {
    std::priority_queue<std::pair<GeoPoint, double>, std::vector<std::pair<GeoPoint, double>>, PriorityQueueCompare> q; //priority queue
    std::unordered_map<std::string, double> distances;          //map to keep track of shortest distances
    std::unordered_map<std::string, GeoPoint> previous;         //map to keep track of previous point in the shortest path

    q.push({pt1, 0});               //initial
    distances[pt1.to_string()] = 0;

    while (!q.empty()) {
        GeoPoint current = q.top().first;   //shortest know distance will be at front
        q.pop();

        if (current == pt2) {                               //if current point is our destination
            std::vector<GeoPoint> path;
            for (GeoPoint point = pt2; point != pt1; ) {        //loop to reconstruct path between points
                path.push_back(point);
                point = previous[point.to_string()];        //set to previous point (connecting shortest path)
            }
            path.push_back(pt1);                        //add to queue and reverse to set path back to correct spot
            std::reverse(path.begin(), path.end());
            return path;
        }

        std::vector<GeoPoint> adjacents = database.get_connected_points(current);       //get connected points
        for (const GeoPoint& next : adjacents) {
            double distance = distances[current.to_string()] + distance_earth_miles(current, next);     //calculate distance
            if (distances.find(next.to_string()) == distances.end() || distance < distances[next.to_string()]) {
                //if this is the shortest known path to the connected point, update containers and distances
                distances[next.to_string()] = distance;
                previous[next.to_string()] = current;
                q.push({next, distance});                   //add connected point to queue with updated distance
            }
        }
    }
    return std::vector<GeoPoint>();  //no path found
}
