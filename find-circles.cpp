#include <nan.h>
#include "iostream"

struct Point {
    unsigned int x;
    unsigned int y;
};

int GetPos(unsigned int& x, unsigned int& y, unsigned int& width) {
    return (y * width + x) * 4; 
}

Point MapCircle(unsigned char* data, unsigned int& x, unsigned int& y,
        unsigned int& width, unsigned int& height, unsigned char& background,
        unsigned char& visited) {
    Point p;
    unsigned int i;
    unsigned int px;
    unsigned int py;
    unsigned int minY;
    unsigned int minX;
    unsigned int maxX;
    unsigned int npx; // negative position x
    unsigned int ppx; // positive position x

    minY = y;
    px = x;
    py = y;
    minX = x;
    maxX = x;

    do {

        i = GetPos(px, py, width);
        if (data[i] == background || py >= height) {
            break;
        }
        data[i + 3] = visited;
        npx = px;
        ppx = px;

        do {
            ppx++;
            i = GetPos(ppx, py, width);
            data[i + 3] = visited;
            if (data[i] == background || ppx >= width) {
                break;
            }
            if (ppx > maxX) {
                maxX = ppx;
            }
        } while (true);

        // go left
        do {
            npx--;
            i = GetPos(npx, py, width);
            data[i + 3] = visited; // before if because we does not support negative values
            if (data[i] == background ) {
                break;
            }
            if (npx < minX) {
                minX = npx; 
            }
            // we can't go before 0
            if (npx == 0) {
                break;
            }
        } while(true);

        px = minX + (maxX - minX) / 2;
        py++;
    } while (true);

    p.x = minX + (maxX - minX) / 2;
    p.y = minY + ((py - 1) - minY) / 2;
    return p;
}

// buffer
// length
// height
// width
// build: ./node_modules/.bin/node-gyp --target=1.7.5 --arch=x64 --dist-url=https://atom.io/download/electron rebuild
/**
 * @param {Buffer} data - Buffer representation of an array
 * @param {Number} width - Image width 
 * @param {Number} height - Image height
 * @param {Number} background - Image background tone from  0 to 255.
 *     TODO: Look how to get a short number
 * @param {Number} visited - Image visited tone from 0 to 255,
 *     use transparency to save visited. TODO: Look how to get a short number.
 *
 * @TODO list:
 *   - Look how return C++ registers as JSON.
 *   - Look best way to manage short numbers.
 */
NAN_METHOD(FindCircles) {
    unsigned char* data = (unsigned char*) node::Buffer::Data(info[0]->ToObject());
    unsigned int width = info[1]->Uint32Value(); // parse to char
    unsigned int height = info[2]->Uint32Value();
    unsigned char background = static_cast<unsigned char>(info[3]->Uint32Value());
    unsigned char visited = static_cast<unsigned char>(info[4]->Uint32Value());
    unsigned int i = 0;
    
    std::vector<Point> points;
    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            i = GetPos(x, y, width);
            if (data[i] != background && data[i + 3] != visited) {
                Point p = MapCircle(data, x, y, width, height, background, visited);
                points.push_back(p);
            }
        }
    }
    
    unsigned int pointBuffLength = 4 * points.size() * 2;
    
    char* pointBuff = new char[pointBuffLength];

    for (std::vector<Point>::iterator i = points.begin(); i != points.end(); ++i) {
        Point p = *i;
        int j = i - points.begin();
        
        pointBuff[j * 8] = (p.x >> 24) & 0xFF;
        pointBuff[j * 8 + 1] = (p.x >> 16) & 0xFF;
        pointBuff[j * 8 + 2] = (p.x >> 8) & 0xFF;
        pointBuff[j * 8 + 3] = p.x & 0xFF;

        pointBuff[j * 8 + 4] = (p.y >> 24) & 0xFF;
        pointBuff[j * 8 + 5] = (p.y >> 16) & 0xFF;
        pointBuff[j * 8 + 6] = (p.y >> 8) & 0xFF;
        pointBuff[j * 8 + 7] = p.y & 0xFF;

    }
    info.GetReturnValue().Set(Nan::NewBuffer(pointBuff, pointBuffLength).ToLocalChecked());
}

NAN_MODULE_INIT(Init) {
    Nan::Set(target, Nan::New<v8::String>("findCircles").ToLocalChecked(),
        Nan::GetFunction(Nan::New<v8::FunctionTemplate>(FindCircles)).ToLocalChecked());
}

NODE_MODULE(my_addon, Init)
