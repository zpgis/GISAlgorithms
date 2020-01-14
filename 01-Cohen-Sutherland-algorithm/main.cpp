#include <iostream>
using namespace std;

class CuhenSutherlandClipper {
private:
	struct Point {
		double x;
		double y;
		Point() = default;
		Point(double x, double y) :x(x), y(y) {
		}
		Point(Point& other) {
			x = other.x;
			y = other.y;
		}		
	};
	struct LineSegment {
		Point* start;	// 端点
		Point* end;		// 端点
		LineSegment() = default;
		// 
		LineSegment(double x0, double y0, double x1, double y1) {
			start = new Point(x0, y0);
			end = new Point(x1, y1);
		}
		LineSegment(LineSegment& other) {			
			start = new Point(*other.start);
			end = new Point(*other.end);
		}
		void setStart(double x, double y) {
			start->x = x;
			start->y = y;
		}
		void setEnd(double x, double y) {
			end->x = x;
			end->y = y;
		}
		double getStartX() const {
			return start->x;
		}
		double getStartY() const {
			return start->y;
		}
		double getEndX() const {
			return end->x;
		}
		double getEndY() const {
			return end->y;
		}
	};
	struct Rectangle {
		double xmin;
		double xmax;
		double ymin;
		double ymax;
		Rectangle() = default;
		// 左下角顶点  矩形宽度、高度
		Rectangle(double xmin, double ymin, double clipw, double cliph) :
			xmin(xmin), ymin(ymin), xmax(xmin + clipw), ymax(ymin + cliph) {}
	};
private:
	LineSegment* lineSegment;
	LineSegment* resultLine;
	Rectangle* rect;
	bool accept;
public:
	CuhenSutherlandClipper() = default;
	// 前面4个参数直线段两端点的坐标，参数5、6是矩形的左下角顶点坐标，7、8是矩形的宽度和长度
	CuhenSutherlandClipper(double x0, double y0, double x1, double y1,
		double clipx, double clipy, double clipw, double cliph) {
		lineSegment = new LineSegment(x0, y0, x1, y1);
		rect = new Rectangle(clipx, clipy, clipw, cliph);
		encodeEndpoint(*lineSegment->start);
		encodeEndpoint(*lineSegment->end);
		resultLine = nullptr;
		accept = false;
	}
	/*
	 * 进行裁剪
	   true代表；该直线段与矩形有交集或存在于矩形内
	   false：表示该线段不在矩形范围内
	 */
	bool lineClipped() {
		//bool accept = false;
		do {
			// get endpoints code
			int e0code = encodeEndpoint(*lineSegment->start); 
			int e1code = encodeEndpoint(*lineSegment->end);

			if ((e0code | e1code )== 0) {
				accept = true;
				resultLine = new LineSegment(*lineSegment);
				break;
			}
			else if ((e0code & e1code) != 0) {
				accept = false;
				break;
			}
			else {
				// 选择一个在裁剪矩形窗口之外的端点
				int code = e0code != 0 ? e0code : e1code;

				double newx, newy;
				double x0 = lineSegment->getStartX();
				double y0 = lineSegment->getStartY();
				double x1 = lineSegment->getEndX();
				double y1 = lineSegment->getEndY();
				// 端点在矩形窗口的左侧
				if ((code & (1 << 0)) != 0) {
					newx = rect->xmin;
					/*
					**    y1- y0 / x1 - x0 = y - y0 / x - x0
					*
					*/
					newy = ((y1 - y0) / (x1 - x0)) * (newx - x0) + y0;
				}
				// 右侧
				else if ((code & (1 << 1)) != 0) {
					newx = rect->xmax;
					newy = ((y1 - y0) / (x1 - x0)) * (newx - x0) + y0;					
				}
				// 下侧
				else if ((code & (1 << 2)) != 0) {
					newy = rect->ymin;
					newx = ((x1 - x0) / (y1 - y0)) * (newy - y0) + x0;
				}
				// 上侧
				else if ((code & (1 << 3)) != 0) {
					newy = rect->ymax;
					newx = ((x1 - x0) / (y1 - y0)) * (newy - y0) + x0;
				}
				/* Now we replace the old endpoint depending on which we chose */
				if (code == e0code) {
					lineSegment->setStart(newx, newy);
				}
				else {
					lineSegment->setEnd(newx, newy);
				}
			}
		} while (true);

		return accept;
	}
	// 如果直线段落在矩形内则输出裁剪之后的直线段，否则输出裁剪为空
	void print() {
		if (accept) {
			cout << "该直线段落在矩形窗口内的部分的坐标为 ：" << endl;
			cout << "x0 = " << resultLine->getStartX() << endl;
			cout << "y0 = " << resultLine->getStartY() << endl;
			cout << "x1 = " << resultLine->getEndX() << endl;
			cout << "y1 = " << resultLine->getEndY() << endl;
		}
		else {
			cout << "直线段并未落在矩形窗口之内" << endl;
		}
	}
	// 更新直线段的坐标
	void setLineSegment(double x0, double y0, double x1, double y1) {
		if (lineSegment != nullptr) {
			lineSegment->setStart(x0, y0);
			lineSegment->setEnd(x1, y1);
		}
		else {
			lineSegment = new LineSegment(x0, y0, x1, y1);
		}
	}
private:
	/*
	 * 对端点值根据矩形进行编码
	 */
	int encodeEndpoint( Point& po) {
		int code = 0;
		if (po.x < rect->xmin) {
			code |= (1 << 0);
		}
		else if (po.x > rect->xmax) {
			code |= (1 << 1);
		}

		if (po.y < rect->ymin) {
			code |= (1 << 2);
		}
		else if (po.y > rect->ymax) {
			code |= (1 << 3);
		}
		return code;
	}

};

int main() {
	CuhenSutherlandClipper clipper(0.5, 1.5, 1, 0, 0, 0, 1, 1);
	if (clipper.lineClipped()) {
		clipper.print();
	}
	else {
		cout << "直线段并未落在矩形窗口之内" << endl;
	}
	clipper.setLineSegment(0.5, 1.5, 2, 3);
	if (clipper.lineClipped()) {
		clipper.print();
	}
	else {
		cout << "直线段并未落在矩形窗口之内" << endl;
	}
	//cout << "hello world" << endl;
	system("pause");
	return 0;
}
