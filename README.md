# Bruin-Nav
Automated tour guide system around UCLA and Westwood that loads and indexes a large number of Open Street Map geospatial data (coordinates of streets) along with lists of tourstops. This data is then used to actually generate the instructions for the tour. 

**Source code is put in a private repository due to school policies, but is available upon request.**


# How It's Made:
Implemented a custom hashmap (that works like the C++ STL unordered map) that mapped streetnames to coordinates around Westwood, which would be used to navigate between points. The Hashmap rehashes its items as more data is added to it. I then created a database class that loads up all the map data (geolocation data) in a text file and then store it in the HashMap data structure. I also impleneted a router class which allows the user to specify a starting geolocation point (latitude/longitude) and ending geolocation point, which then generates a vector of geolocation points that describes an efficient path from the starting point to the ending point. The data stored from these classes would then be used to produce the tour instructions to follow, providing commentary and walking directions. Time complexity/Big-O is taken into account for all functions of the classes. 
  
Tech Used: C++, XCode
