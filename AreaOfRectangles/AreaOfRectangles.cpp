#include "pch.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
using namespace std;

struct range { int low, high; };

class area_height_calculator {
	struct tree_node { int h, sum, cnt; };
	vector<tree_node> tree;
	int node_count;

public:
	area_height_calculator(vector<range>& v) 
		: node_count((int)v.size()) { 
		init(v); 
	}
	~area_height_calculator() {}

	int update(int start, int end, const int cnt) { 
		return update(start, end, cnt, 0, 0, node_count - 1); 
	}
	int height() { return tree[0].sum; }

private:
	int calc_mid(int left, int right) { return (left + right) / 2; }

	void init(vector<range>& v) {
		int h = (int)(ceil(log2(node_count)));
		int size = 2 * (int)pow(2, h) - 1;
		tree.resize(size);
		build_tree(v, 0, 0, node_count - 1);
	}

	int build_tree(vector<range>& v, int node_index, int left, int right) {
		if (left == right) {
			tree[node_index].h = v[left].high - v[left].low;
			return tree[node_index].h;
		}

		int mid = calc_mid(left, right);
		tree[node_index].h = build_tree(v, node_index * 2 + 1, left, mid) + 
			                 build_tree(v, node_index * 2 + 2, mid + 1, right);
		return tree[node_index].h;
	}

	int update(int start, int end, const int cnt, int node_index, int left, int right) {
		tree_node& node = tree[node_index];
		if ((left < start && right < start) || (left > end && right > end))
			return node.sum;

		if (left >= start && right <= end) {
			node.cnt += cnt;
		}
		else {
			int mid = calc_mid(left, right);
			node.sum = update(start, end, cnt, node_index * 2 + 1, left, mid) + 
				       update(start, end, cnt, node_index * 2 + 2, mid + 1, right);
		}

		if (node.cnt == 0) {
			if (left == right)
				node.sum = 0;
			else
				node.sum = tree[node_index * 2 + 1].sum + tree[node_index * 2 + 2].sum;
		}
		else {
			node.sum = node.h;
		}

		return node.sum;
	}
};

class area_of_rectangles
{
public:
	static bool compare_low(const range& a, int val) { return a.low < val; }
	static bool compare_high(const range& a, int val) { return a.high < val; }

	long long calculate(vector<vector<int>>& rectangles) {
		auto size = rectangles.size();
		struct y_info { int y, cnt; };
		vector<y_info> v_y_info(size * 2);
		int idx = 0;
		for (auto& r : rectangles) {
			v_y_info[idx] = { r[1], +1 }; idx++;
			v_y_info[idx] = { r[3], -1 }; idx++;
		}
		sort(v_y_info.begin(), v_y_info.end(), [](y_info& a, y_info& b) { return a.y < b.y; });

		vector<range> v_range;
		int prev_y = 0, cnt = 0;
		for (auto& info : v_y_info) {
			if (cnt > 0 && prev_y != info.y)
				v_range.push_back({ prev_y, info.y });
			prev_y = info.y; cnt += info.cnt;
		}

		struct sweep_event { int x, start, end, cnt; };
		vector<sweep_event> v_event(size * 2);
		int start, end; idx = 0;
		for (auto& r : rectangles) {
			start = distance(v_range.begin(), lower_bound(v_range.begin(), v_range.end(), r[1], compare_low));
			end = distance(v_range.begin(), lower_bound(v_range.begin(), v_range.end(), r[3], compare_high));
			v_event[idx] = { r[0], start, end, +1 }; idx++;
			v_event[idx] = { r[2], start, end, -1 }; idx++;
		}
		sort(v_event.begin(), v_event.end(), [](const sweep_event& a, const sweep_event& b) {
			return a.x != b.x ? a.x < b.x : a.cnt > b.cnt;
		});

		long long area = 0, prev_x = 0;
		area_height_calculator c(v_range);
		for (auto& e : v_event) {
			area += (e.x - prev_x) * c.height();
			prev_x = e.x;
			c.update(e.start, e.end, e.cnt);
		}

		return area;
	}
};


int main()
{
	auto rect = vector<int>(); // left, bottom, right, top
	auto rectangles = vector<vector<int>>();

	rect = { 1, 1, 6, 5 };
	rectangles.push_back(rect);

	rect = { 2, 0, 4, 2};
	rectangles.push_back(rect);

	rect = { 2, 4, 5, 7 };
	rectangles.push_back(rect);

	rect = { 4, 3, 8, 6 };
	rectangles.push_back(rect);

	rect = { 7, 5, 9, 7};
	rectangles.push_back(rect);

	area_of_rectangles ar;
	std::cout << "Area of rectangles = " << ar.calculate(rectangles) << endl;
}
