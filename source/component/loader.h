#ifndef LOADER_H
#define LOADER_H

#include <QDebug>
#include <QColor>

class MainWindow;
class Wpe;
class Mapping;
class Remapping;
class ColorCycling;

class ComponentLoader : public QObject
{
    Q_OBJECT

private:
    MainWindow *mw;

    /* components
     * and index of current selected one of them */
    QVector<Wpe *> wpes;
    int icWpe;

    QVector<Mapping *> mappings;
    int icMapping;

    QVector<Remapping *> remappings;
    int icRemapping;

    QVector<ColorCycling *> colorCyclings;
    int icColorCycling;

    QVector<QRgb> colorTable;

    void loadWpe();
    void loadMapping();
    void loadRemapping();
    void loadColorCycling();

    QRgb overflowedColor;

    static const int CYCLING_PERIOD = 10;

public:
    explicit ComponentLoader(MainWindow *mw);

    QVector<Wpe *> const &getWpes() const {return wpes;}
    QVector<Mapping *> const &getMappings() const {return mappings;}
    QVector<Remapping *> const &getRemappings() const {return remappings;}
    QVector<ColorCycling *> const &getColorCyclings() const {return colorCyclings;}

    int getWpeIndex () {return icWpe;}
    int getMappingIndex () {return icMapping;}
    int getRemappingIndex () {return icRemapping;}
    int getColorCyclingIndex () {return icColorCycling;}

    QVector<QRgb> const &getColorTable() const;
    void updateColorTable ();

signals:
    void colorTableChanged();

public slots:
    void setWpeIndex (int i) {
#ifdef QT_DEBUG
        qDebug() << "SLOT ComponentLoader::setWpeIndex";
#endif
        if (icWpe == i)
            return;
        icWpe = i;

        updateColorTable();
    }
    void setMappingIndex (int i) {
#ifdef QT_DEBUG
        qDebug() << "SLOT ComponentLoader::setMappingIndex";
#endif
        if (icMapping == i)
            return;
        icMapping = i;

        updateColorTable();
    }
    void setRemappingIndex (int i) {
#ifdef QT_DEBUG
        qDebug() << "SLOT ComponentLoader::setRemappingIndex";
#endif
        if (icRemapping == i)
            return;
        icRemapping = i;

        updateColorTable();
    }
    void setColorCyclingIndex (int i) {
#ifdef QT_DEBUG
        qDebug() << "SLOT ComponentLoader::setColorCyclingIndex";
#endif
        if (icColorCycling == i)
            return;
        icColorCycling = i;

        updateColorTable();
    }
    void cyclingTick();
};

#endif // LOADER_H
