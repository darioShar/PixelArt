#include "spline.h"
#include "voronoi.h"



//Returns the darker pixel by Y luminescence value -> A MODIFIER
Pixel* darker(Pixel* a, Pixel* b)
{
    ColorYUV color1 = a->color();
    ColorYUV color2 = b->color();
    if(color1.Y < color2.Y) return a;
    else return b;
}



//Extracts active edges from voronoi diagrams
void Spline::fillActiveEdges()
{
    if(this->diagram == nullptr) return;
    
    //For keeping the edges detected till now.
    std::map<Edge,Pixel*> edgeEnum;

    //Get image dimensions
    Image* imageRef = this->diagram->getImage(); 
    int width = imageRef->getWidth();
    int height = imageRef->getHeight();

    for(int x=0; x <width; x++)
    {
        for(int y=0; y< height; y++)
        {
            for(int i = 0 ; i < (*this->diagram)(x,y).size(); i++) 
            {

                //Looping over the voronoi hull, of point (x,y), we find the edges which have different colored pixels on either side, and add to active edges.
                int l = i;
                int r = (i+1)%((*this->diagram)(x,y).size());
                if(edgeEnum.find(make_pair((*this->diagram)(x,y)[r],(*this->diagram)(x,y)[l])) != edgeEnum.end()) 
                {
                    auto p = edgeEnum[make_pair((*this->diagram)(x,y)[r],(*this->diagram)(x,y)[l])];
                    if(p != (*imageRef)(x,y) && !(p->color() == (*imageRef)(x,y)->color())) activeEdges.push_back(make_pair(make_pair((*this->diagram)(x,y)[l],(*this->diagram)(x,y)[r]),darker(p,(*imageRef)(x,y)))); 
                }
                else edgeEnum[std::make_pair((*this->diagram)(x,y)[l],(*this->diagram)(x,y)[r])] = (*imageRef)(x,y);
            }
        }
    }
}

void Spline::fillGraph()
{
    for(auto edge : activeEdges)
    {
        auto P1 = std::make_pair(edge.first.second,edge.second->color());
        auto P2 = std::make_pair(edge.first.first,edge.second->color());

        const auto x = edge.first;

        graph[x.first].insert(P1);
        graph[x.second].insert(P2);
    }
}

//A MODIFIER
std::vector<std::pair<std::vector<Point>,Color> > Spline::printGraph()
{
    //Tracing curves. Starting with a random node, We trace out a curve with same colors
    std::vector<std::pair<std::vector<Point>, Color> > mainOutLine;
    std::map<Point, std::set<std::pair<Point,Color> > >::iterator vertexPt = graph.begin();
    while(vertexPt != graph.end())
    {
        while((vertexPt->second).size() > 0)
        {
            Point src = (vertexPt->second).begin()->first;
            Color c = (vertexPt->second).begin()->second;
            std::vector<Point> v = traverseGraph(src, c);
            mainOutLine.push_back(std::make_pair(v,c));
        }
        vertexPt ++;
    }
    return mainOutLine;
}



//ALGORITHM FROM https://www.programiz.com/dsa/graph-bfs
//A RECODER !!
std::vector<Point> Spline::traverseGraph(const Point& q,const Color& c)
{
    std::vector<Point> visited;
    Point current_point = q;
    Point prev = Point(-1,-1);
    Color curr = c;
    bool found=true;

    while(found)
    {
        visited.push_back(current_point);
        for(auto it = graph[x].begin();it!=graph[x].end();it++)
        {
            if(it->first == prev) continue;
            if(it->second == c)
            {
                Point p2 = it->first;
                std::set<std::pair<Point,Color>>::iterator it1;
                for(it1 = graph[p2].begin();it1!=graph[p2].end();it1++){
                    if(curr == it1 -> second){
                        if(it1->first==current_point) break;
                    }
                }

                if(it1 == graph[p2].end()) break;
                curr = it -> second;
                graph[current_point].erase(it);
                graph[p2].erase(it1)

                current_point = p2;
                found = True;:
                break;
            }

        }
        if(!found) break;
        found = false;
    }

    if(visited.size() > 2 && *visited.begin() == *visited.rbegin()){
        visited.push_back(visited[1]);
    }
    return visited;
}


//TRANSFORME OK
std::vector<std::vector<float> > Spline::getSpline(std::vector<Point> points) // For 3 points
{
    assert(points.size()==3);

    //Basis Matrix for spline interpolation with quadratic norm
    std::vector<std::vector<float>> M = {{1/2, 1/2, 0},{ -1, 1, 0},{1/2, -1, 1/2}};

    //initialize the vector vec
    std::vector<std::vector<float>> vec(3, std::vector<float>(2,0.0));

    //iterate using B-spline matrix
    for(int i = 0 ; i < 3; i++)
    {
        for(int j = 0; j < 2; j++)
        {
            for(int k = 0 ; k < 3; k ++) 
            {
                if(j == 0){
                    vec[i][j] +=  M[i][k]*points[k].first;
                }
                if(j==1){
                    vec[i][j] += M[i][k]*points[k].second;
                }
            }
        }
    }
    return vec;
}