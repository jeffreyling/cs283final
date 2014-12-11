function shape_tree = contour_to_shape_tree(contour)
% A shape tree is a binary tree represented by an array. Left child of idx
% is 2*idx, right child is 2*idx + 1
% Each node must have:
%   left child
%   right child
%   coordinates of midpoint wrt to endpoints

contour = reshape(contour, [size(contour,2), 2]);
start_v = contour(1,:);
end_v = contour(2,:);