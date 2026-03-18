#include "graphview.h"

#include <QMouseEvent>
#include <QPainter>
#include <QWheelEvent>

GraphView::GraphView(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptHoverEvents(true);
    setFlag(QQuickItem::ItemHasContents, true);
}

GraphManager *GraphView::manager() const
{
    return m_manager;
}

void GraphView::setManager(GraphManager *manager)
{
    if (m_manager == manager) {
        return;
    }

    if (m_manager) {
        disconnect(m_manager, nullptr, this, nullptr);
    }

    m_manager = manager;
    connectManagerSignals();
    rebuildCache();
    emit managerChanged();
}

void GraphView::paint(QPainter *painter)
{
    if (m_cachedImage.isNull()) {
        rebuildCache();
    }

    painter->drawImage(boundingRect(), m_cachedImage);
}

void GraphView::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickPaintedItem::geometryChanged(newGeometry, oldGeometry);
    if (newGeometry.size() != oldGeometry.size()) {
        rebuildCache();
    }
}

void GraphView::mousePressEvent(QMouseEvent *event)
{
    if (!m_manager) {
        event->ignore();
        return;
    }

    const QRectF graphArea = m_manager->renderer()->chartRect(boundingRect().size().toSize());
    if (!graphArea.contains(event->localPos())) {
        m_manager->clearSelection();
        event->accept();
        return;
    }

    m_manager->selectNearestPointX(graphArea, event->localPos().x());
    event->accept();
}

void GraphView::wheelEvent(QWheelEvent *event)
{
    if (!m_manager) {
        event->ignore();
        return;
    }

    const qreal factor = event->angleDelta().y() > 0 ? 0.85 : 1.15;
    const qreal centerRatio = width() > 0 ? event->position().x() / width() : 0.5;
    m_manager->zoomX(factor, centerRatio);
    event->accept();
}

void GraphView::rebuildCache()
{
    if (!m_manager || width() <= 0 || height() <= 0) {
        m_cachedImage = {};
        update();
        return;
    }

    m_cachedImage = m_manager->renderer()->render(boundingRect().size().toSize(),
                                                  m_manager->seriesModel()->series(),
                                                  m_manager->xAxis(),
                                                  m_manager->yAxis(),
                                                  m_manager->selectedX(),
                                                  m_manager->hasSelection());
    update();
}

void GraphView::connectManagerSignals()
{
    if (!m_manager) {
        return;
    }

    connect(m_manager, &GraphManager::seriesChanged, this, &GraphView::rebuildCache);
    connect(m_manager, &GraphManager::xAxisChanged, this, &GraphView::rebuildCache);
    connect(m_manager, &GraphManager::yAxisChanged, this, &GraphView::rebuildCache);
    connect(m_manager, &GraphManager::selectionChanged, this, &GraphView::rebuildCache);
}
