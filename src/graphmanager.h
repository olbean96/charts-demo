#pragma once

#include "axisconfig.h"
#include "graphseriesmodel.h"
#include "graphrenderer.h"
#include "legendmodel.h"

#include <QObject>

/*!
 * \brief Основной менеджер модуля графиков и внешний API для работы с ним.
 */
class GraphManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(GraphSeriesModel *seriesModel READ seriesModel CONSTANT)
    Q_PROPERTY(LegendModel *legendModel READ legendModel CONSTANT)
    Q_PROPERTY(qreal xMinimum READ xMinimum NOTIFY xAxisChanged)
    Q_PROPERTY(qreal xMaximum READ xMaximum NOTIFY xAxisChanged)
    Q_PROPERTY(qreal yMinimum READ yMinimum NOTIFY yAxisChanged)
    Q_PROPERTY(qreal yMaximum READ yMaximum NOTIFY yAxisChanged)
    Q_PROPERTY(bool hasSelection READ hasSelection NOTIFY selectionChanged)
    Q_PROPERTY(qreal selectedX READ selectedX NOTIFY selectionChanged)

public:
    /*!
     * \brief Создает менеджер графиков.
     * \param parent Родительский объект Qt.
     */
    explicit GraphManager(QObject *parent = nullptr);

    /*!
     * \brief Возвращает модель графиков.
     */
    GraphSeriesModel *seriesModel() const;

    /*!
     * \brief Возвращает модель легенды.
     */
    LegendModel *legendModel() const;

    /*!
     * \brief Возвращает текущие настройки оси X.
     */
    AxisConfig xAxis() const;

    /*!
     * \brief Возвращает текущие настройки оси Y.
     */
    AxisConfig yAxis() const;

    /*!
     * \brief Возвращает минимальное значение оси X.
     */
    qreal xMinimum() const;

    /*!
     * \brief Возвращает максимальное значение оси X.
     */
    qreal xMaximum() const;

    /*!
     * \brief Возвращает минимальное значение оси Y.
     */
    qreal yMinimum() const;

    /*!
     * \brief Возвращает максимальное значение оси Y.
     */
    qreal yMaximum() const;

    /*!
     * \brief Возвращает признак наличия выбранной точки.
     */
    bool hasSelection() const;

    /*!
     * \brief Возвращает выбранное значение по оси X.
     */
    qreal selectedX() const;

    /*!
     * \brief Возвращает объект отрисовщика.
     */
    GraphRenderer *renderer() const;

    /*!
     * \brief Добавляет новый график.
     * \param id Идентификатор графика.
     * \param name Название графика.
     * \param color Цвет линии.
     * \param lineStyle Стиль соединения точек.
     * \return Созданный график либо существующий график с тем же идентификатором.
     */
    Q_INVOKABLE QObject *addSeries(const QString &id, const QString &name, const QColor &color, int lineStyle = 0);

    /*!
     * \brief Удаляет график.
     * \param id Идентификатор графика.
     * \return true, если график найден и удален.
     */
    Q_INVOKABLE bool removeSeries(const QString &id);

    /*!
     * \brief Полностью заменяет точки графика.
     * \param id Идентификатор графика.
     * \param points Новый набор точек.
     * \return true, если график найден.
     */
    bool setSeriesPoints(const QString &id, const QVector<GraphPoint> &points);

    /*!
     * \brief Устанавливает стиль соединения точек для графика.
     * \param id Идентификатор графика.
     * \param lineStyle Новый стиль линии.
     * \return true, если график найден.
     */
    Q_INVOKABLE bool setSeriesLineStyle(const QString &id, int lineStyle);

    /*!
     * \brief Устанавливает диапазон и шаг оси X.
     * \param minimum Минимум оси.
     * \param maximum Максимум оси.
     * \param tickStep Шаг меток.
     * \param title Подпись оси.
     */
    Q_INVOKABLE void setXAxis(qreal minimum, qreal maximum, qreal tickStep, const QString &title);

    /*!
     * \brief Устанавливает диапазон и шаг оси Y.
     * \param minimum Минимум оси.
     * \param maximum Максимум оси.
     * \param tickStep Шаг меток.
     * \param title Подпись оси.
     */
    Q_INVOKABLE void setYAxis(qreal minimum, qreal maximum, qreal tickStep, const QString &title);

    /*!
     * \brief Выполняет масштабирование по оси X относительно центральной точки.
     * \param factor Коэффициент масштаба.
     * \param centerRatio Нормированная позиция центра масштабирования в диапазоне 0..1.
     */
    Q_INVOKABLE void zoomX(qreal factor, qreal centerRatio);

    /*!
     * \brief Ищет ближайшую точку по оси X и активирует выделение.
     * \param graphArea Область построения.
     * \param clickX Экранная координата клика по оси X.
     */
    Q_INVOKABLE void selectNearestPointX(const QRectF &graphArea, qreal clickX);

    /*!
     * \brief Сбрасывает текущее выделение.
     */
    Q_INVOKABLE void clearSelection();

    /*!
     * \brief Заполняет менеджер тестовыми данными.
     */
    Q_INVOKABLE void createDemoData();

signals:
    /*!
     * \brief Сигнал отправляется при изменении набора графиков или их данных.
     */
    void seriesChanged();

    /*!
     * \brief Сигнал отправляется при изменении настроек оси X.
     */
    void xAxisChanged();

    /*!
     * \brief Сигнал отправляется при изменении настроек оси Y.
     */
    void yAxisChanged();

    /*!
     * \brief Сигнал отправляется при изменении выбора точки.
     */
    void selectionChanged();

    /*!
     * \brief Сигнал отправляется при выборе новой координаты по оси X.
     * \param x Значение выбранной координаты.
     */
    void xValueSelected(qreal x);

protected:
    /*!
     * \brief Подключает сигналы графика к сигналам менеджера.
     * \param series График для подключения.
     */
    void attachSeries(GraphSeries *series);

private:
    GraphSeriesModel m_seriesModel;
    LegendModel m_legendModel;
    GraphRenderer m_renderer;
    AxisConfig m_xAxis;
    AxisConfig m_yAxis;
    bool m_hasSelection = false;
    qreal m_selectedX = 0.0;
};
