#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <list>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point_2;
typedef CGAL::Polygon_2<K> Polygon_2;
typedef CGAL::Polygon_with_holes_2<K> Polygon_with_holes_2;

int main() {
    // Create first polygon (square)
    Polygon_2 poly1;
    poly1.push_back(Point_2(0, 0));
    poly1.push_back(Point_2(2, 0));
    poly1.push_back(Point_2(2, 2));
    poly1.push_back(Point_2(0, 2));
    
    // Create second polygon (overlapping square)
    Polygon_2 poly2;
    poly2.push_back(Point_2(1, 1));
    poly2.push_back(Point_2(3, 1));
    poly2.push_back(Point_2(3, 3));
    poly2.push_back(Point_2(1, 3));
    
    std::list<Polygon_with_holes_2> result;
    
    // Union
    CGAL::join(poly1, poly2, std::back_inserter(result));
    std::cout << "Union result has " << result.size() << " polygons" << std::endl;
    
    result.clear();
    
    // Intersection
    CGAL::intersection(poly1, poly2, std::back_inserter(result));
    std::cout << "Intersection result has " << result.size() << " polygons" << std::endl;
    
    result.clear();
    
    // Difference
    CGAL::difference(poly1, poly2, std::back_inserter(result));
    std::cout << "Difference result has " << result.size() << " polygons" << std::endl;
    
    return 0;
}