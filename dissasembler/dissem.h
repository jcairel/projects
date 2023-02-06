#include <iomanip>
class Dissem{
public:
    void open_File(char *obj_File, char *sim_File);
    void disassemble();
    private:
    void load_Data();
     void header_Check(int row);
    void text_Check(int row);
    void modify_Check(int row);
    void end_Check(int row);
}
;
