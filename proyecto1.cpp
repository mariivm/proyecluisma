#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Text_Display.H>
#include <FL/fl_draw.H>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip> // Para std::setprecision

// Widget personalizado para el box plot
class BoxPlot : public Fl_Widget {
    std::vector<double> data;

public:
    BoxPlot(int X, int Y, int W, int H, const std::vector<double>& data)
        : Fl_Widget(X, Y, W, H), data(data) {}

    void draw() {
        if (data.empty()) return;

        std::sort(data.begin(), data.end());
        double q1 = data[data.size() / 4];
        double median = data[data.size() / 2];
        double q3 = data[data.size() * 3 / 4];
        double iqr = q3 - q1;
        double lower_whisker = q1 - 1.5 * iqr;
        double upper_whisker = q3 + 1.5 * iqr;

        // Asegurarse de que los bigotes no excedan los valores reales de los datos
        auto low = std::lower_bound(data.begin(), data.end(), lower_whisker);
        auto high = std::upper_bound(data.begin(), data.end(), upper_whisker);
        lower_whisker = (low == data.end()) ? q1 : *low;
        upper_whisker = (high == data.begin()) ? q3 : *(high - 1);

        int mid_y = y() + h() / 2;
        int box_width = w() / 3;
        int box_left = x() + w() / 3;

        // Dibuja el cuerpo del box plot
        fl_color(FL_BLACK);
        fl_rectf(box_left, mid_y - 10, box_width, 20);  

        // Dibuja la mediana
        fl_color(FL_RED);
        fl_line(box_left + box_width / 2, mid_y - 10, box_left + box_width / 2, mid_y + 10);  

        // Dibuja los bigotes
        fl_color(FL_BLACK);
        fl_line(box_left, mid_y, box_left - 30, mid_y);  
        fl_line(box_left + box_width, mid_y, box_left + box_width + 30, mid_y);  
        fl_line(box_left - 30, mid_y - 5, box_left - 30, mid_y + 5);  
        fl_line(box_left + box_width + 30, mid_y - 5, box_left + box_width + 30, mid_y + 5);  

        // Dibuja los valores numéricos debajo del gráfico
        fl_font(FL_HELVETICA_BOLD, 14);
        fl_color(FL_WHITE); // Fondo blanco detrás de los números para mejor legibilidad

        // Dibujar Q1
        std::string q1_str = std::to_string(q1);
        fl_color(FL_BLACK);
        fl_draw(q1_str.c_str(), box_left + 20, mid_y + 35);

        // Dibujar mediana
        std::string median_str = std::to_string(median);
        fl_color(FL_BLACK);
        fl_draw(median_str.c_str(), box_left + box_width / 2 - 30, mid_y + 35);

        // Dibujar Q3
        std::string q3_str = std::to_string(q3);
        fl_color(FL_BLACK);
        fl_draw(q3_str.c_str(), box_left + box_width - 60, mid_y + 35);
    }

    void setData(const std::vector<double>& newData) {
        data = newData;
        redraw();
    }
};

Fl_Text_Buffer *buffer = new Fl_Text_Buffer();
Fl_Text_Display *display;
std::vector<double> datos;
BoxPlot *boxPlot;

void cargar_datos_callback(Fl_Widget* widget, void* data) {
    buffer->text("Iniciando carga de datos...\n");
    Fl_File_Chooser chooser(".", "*.csv", Fl_File_Chooser::SINGLE, "Seleccionar archivo CSV");
    chooser.show();
    while (chooser.shown())
        Fl::wait();

    if (chooser.value() != nullptr) {
        std::stringstream message;
        message << "Archivo seleccionado: " << chooser.value() << "\n";
        buffer->append(message.str().c_str());

        std::ifstream archivo(chooser.value());
        if (!archivo) {
            buffer->append("Error al abrir el archivo.\n");
            return;
        }

        std::string linea;
        datos.clear();  // Limpia los datos antiguos
        while (std::getline(archivo, linea)) {
            std::stringstream ss(linea);
            double num;
            while (ss >> num) {
                datos.push_back(num);
                if (ss.peek() == ',') ss.ignore();
            }
        }
        archivo.close();
        buffer->append("Éxito al cargar los datos.\n");

        boxPlot->setData(datos);  // Actualiza el box plot
    } else {
        buffer->append("No se seleccionó ningún archivo.\n");
    }
    display->redraw();
}

int main() {
    Fl_Window ventana(800, 600, "Cargar datos desde archivo CSV");
    Fl_Button boton(150, 10, 100, 30, "Cargar datos");
    boton.callback(cargar_datos_callback);

    display = new Fl_Text_Display(10, 50, 780, 100);
    display->buffer(buffer);

    datos = std::vector<double>();
    boxPlot = new BoxPlot(50, 200, 700, 300, datos);  // Ajusta el tamaño del widget BoxPlot
    
    ventana.end();
    ventana.show();
    
    return Fl::run();
}
