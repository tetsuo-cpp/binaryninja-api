#pragma once

#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTabBar>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QAbstractScrollArea>
#include <QtGui/QMouseEvent>
#include "uitypes.h"


class BINARYNINJAUIAPI DockableTabBar: public QAbstractScrollArea
{
	Q_OBJECT

	struct TabInfo
	{
		QString title;
		QString toolTip;
		QRect tabRect, closeButtonRect, closeIconRect;
	};

	std::vector<TabInfo> m_tabs;
	int m_active = -1;

	bool m_mouseInside = false;
	int m_closeButtonHover = -1;
	int m_closeButtonDown = -1;
	QTimer* m_timer;

	QStyleOptionTab styleForTab(int idx, const TabInfo& info) const;
	QSize sizeForTab(const QStyleOptionTab& tabStyle, const TabInfo& info) const;
	int closeButtonSize() const;
	void updateLayout();

public:
	DockableTabBar(QWidget* parent = nullptr);

	int addTab(const QString& title);
	int insertTab(int idx, const QString& title);
	void removeTab(int idx);
	void setCurrentIndex(int idx);
	void setTabText(int idx, const QString& title);
	void setTabToolTip(int idx, const QString& toolTip);

	int count() const;
	int currentIndex() const;
	int tabAt(const QPoint& pt);
	QRect tabRect(int idx);
	QString tabText(int idx);

	void ensureCurrentTabVisible();

Q_SIGNALS:
	void currentChanged(int idx);
	void tabCloseRequested(int idx);

private Q_SLOTS:
	void underMouseTimerEvent();

protected:
	virtual QSize sizeHint() const override;
	virtual QSize minimumSizeHint() const override;
	virtual void paintEvent(QPaintEvent* event) override;
	virtual void enterEvent(QEnterEvent* event) override;
	virtual void leaveEvent(QEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;
	virtual void resizeEvent(QResizeEvent* event) override;
	virtual void wheelEvent(QWheelEvent* event) override;
};

class BINARYNINJAUIAPI DockableTabWidget: public QWidget
{
	Q_OBJECT

	DockableTabBar* m_bar;
	QHBoxLayout* m_barLayout;
	QStackedWidget* m_widgets;

public:
	DockableTabWidget(QWidget* parent = nullptr);

	int addTab(QWidget* widget, const QString& title);
	int insertTab(int idx, QWidget* widget, const QString& title);
	void removeTab(int idx);

	int count();
	int currentIndex();
	QWidget* currentWidget();
	QWidget* widget(int idx);
	DockableTabBar* tabBar() const { return m_bar; }
	QString tabText(int idx);
	int indexOf(QWidget* widget);

	void setCurrentIndex(int idx);
	void setTabText(int idx, const QString& title);

	void setCornerWidget(QWidget* widget, Qt::Corner corner = Qt::TopRightCorner);

protected:
	virtual void paintEvent(QPaintEvent* event) override;

Q_SIGNALS:
	void tabCloseRequested(int idx);
	void currentChanged(int idx);

private Q_SLOTS:
	void tabBarCurrentChanged(int idx);
	void tabBarCloseRequested(int idx);
};
