template <typename T>
class vector {
private:
    T* buffer;
    int capacity;
    int gapStart;
    int gapEnd;

public:
    vector(int size = 1024 * 1024 * 10) : capacity(size), gapStart(0), gapEnd(size) {
        buffer = new T[size];
    }
    ~vector() { delete[] buffer; }

    int end() { return size(); } //
    int begin() { return 0; }    

    void moveGap(int pos) {
        if (pos < gapStart) {
            while (gapStart > pos) buffer[--gapEnd] = buffer[--gapStart];
        } else if (pos > gapStart) {
            while (gapStart < pos) buffer[gapStart++] = buffer[gapEnd++];
        }
    }

    void insert(int pos, T val) {
        moveGap(pos);
        if (gapStart < gapEnd) buffer[gapStart++] = val;
    }

    void erase(int pos) {
        moveGap(pos);
        if (gapEnd < capacity) gapEnd++;
    }

    bool empty() { return (gapStart + (capacity - gapEnd)) == 0; }
    int size() { return gapStart + (capacity - gapEnd); }

    
    T* data() { return buffer; } 
    
    
    std::string to_string() {
        std::string s;
        s.reserve(size());
        s.append(buffer, gapStart);
        s.append(buffer + gapEnd, capacity - gapEnd);
        return s;
    }
};