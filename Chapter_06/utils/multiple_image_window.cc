#include "multiple_image_window.h"

MultipleImageWindow::MultipleImageWindow(const string &window_title, int cols, int rows, int flags) {
  this->window_title = window_title;
  this->cols = cols;
  this->rows = rows;
  namedWindow(window_title, flags);
  moveWindow(window_title, 0, 0);
  this->canvas_width = 1200;
  this->canvas_height = 700;
  this->canvas = Mat(this->canvas_height, this->canvas_width, CV_8UC3);
  imshow(this->window_title, this->canvas);
}

int MultipleImageWindow::addImage(const string &title, const Mat &image, bool render) {
  // Check if the title exists in the vector titles
  int index = -1;
  for (int i = 0; i < int(this->titles.size()); i++) {
    string t = this->titles[i];
    if (t == title) {
      index = i;
      break;
    }
  }
  if (index == -1) {
    this->titles.push_back(title);
    this->images.push_back(image);
  } else {
    this->images[index] = image;
  }
  if (render)
    this->render();
  return int(this->images.size()) - 1;
}

void MultipleImageWindow::removeImage(int pos) {
  this->titles.erase(this->titles.begin() + pos);
  this->images.erase(this->images.begin() + pos);
}

void MultipleImageWindow::render() {
  // Clean our canvas
  this->canvas.setTo(Scalar(20, 20, 20));
  // width and height of cell. add 10 px of padding between images
  int cell_width = (canvas_width / cols);
  int cell_height = (canvas_height / rows);
  int max_images = (this->images.size() > cols * rows) ? cols * rows : int(this->images.size());
  int i = 0;
  auto titles_it = this->titles.begin();
  for (const auto &img : this->images) {
    string title = *titles_it;
    int cell_x = (cell_width) * ((i) % cols); // A
    int cell_y = int(float(cell_height) * floor(float(i) / (float) cols));
    Rect mask(cell_x, cell_y, cell_width, cell_height);

    //For each cell draw an image if exists
    Mat cell(this->canvas, mask);
    // resize image to cell size
    Mat resized;
    double cell_aspect = (double) cell_width / (double) cell_height;
    double img_aspect = (double) img.cols / (double) img.rows;
    double f = (cell_aspect < img_aspect) ? (double) cell_width / (double) img.cols : (double) cell_height /
                                                                                      (double) img.rows;
    resize(img, resized, Size(0, 0), f, f);
    if (resized.channels() == 1) {
      cvtColor(resized, resized, COLOR_GRAY2BGR);
    }

    // Assign the image
    //Mat sub_cell(this->canvas, Rect(cell_x+1,cell_y+1,resized.cols, resized.rows));
    Mat sub_cell(this->canvas,
                 Rect(cell_x + ((cell_width - resized.cols) / 2), cell_y + ((cell_height - resized.rows) / 2),
                      resized.cols, resized.rows));
    resized.copyTo(sub_cell);

    putText(cell, title, Point(20, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(200, 0, 0), 1, LINE_AA);

    // Draw a rectangle for each cell mat
    rectangle(canvas, Rect(cell_x, cell_y, cell_width, cell_height), Scalar(200, 200, 200), 1);

    i++;
    ++titles_it;
    if (i == max_images)
      break;
  }

  // show image
  imshow(this->window_title, this->canvas);
}

