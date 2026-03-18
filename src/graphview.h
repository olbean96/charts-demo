#pragma once

#include "graphmanager.h"

#include <QImage>
#include <QQuickPaintedItem>

/*!
 * \brief QML-элемент отображения графиков с интерактивным выбором и масштабированием.
 */
class GraphView : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(GraphManager *manager READ manager WRITE setManager NOTIFY managerChanged)

public:
    /*!
     * \brief Создает элемент отображения графиков.
     * \param parent Родительский элемент сцены.
     */
    explicit GraphView(QQuickItem *parent = nullptr);

    /*!
     * \brief Возвращает менеджер графиков.
     */
    GraphManager *manager() const;

    /*!
     * \brief Устанавливает менеджер графиков.
     * \param manager Менеджер графиков.
     */
    void setManager(GraphManager *manager);

    /*!
     * \brief Выполняет отрисовку содержимого элемента.
     * \param painter Объект рисования Qt.
     */
    void paint(QPainter *painter) override;

signals:
    /*!
     * \brief Сигнал отправляется при смене менеджера графиков.
     */
    void managerChanged();

protected:
    /*!
     * \brief Обрабатывает изменение геометрии элемента.
     * \param newGeometry Новая геометрия.
     * \param oldGeometry Предыдущая геометрия.
     */
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;

    /*!
     * \brief Обрабатывает нажатие мыши по области графика.
     * \param event Событие мыши.
     */
    void mousePressEvent(QMouseEvent *event) override;

    /*!
     * \brief Обрабатывает прокрутку колесика мыши для масштабирования.
     * \param event Событие колесика.
     */
    void wheelEvent(QWheelEvent *event) override;

private:
    void rebuildCache();
    void connectManagerSignals();

    GraphManager *m_manager = nullptr;
    QImage m_cachedImage;
};
