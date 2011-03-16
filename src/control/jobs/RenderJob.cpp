#include "RenderJob.h"
#include <gtk/gtk.h>
#include "../../gui/XournalView.h"
#include "../../view/PdfView.h"
#include "../../view/DocumentView.h"
#include "../../gui/PageView.h"
#include "../../model/Document.h"
#include "../../util/Rectangle.h"

RenderJob::RenderJob(PageView * view) {
	this->view = view;
	this->repaintComplete = false;
	this->repaintRect = NULL;
}

RenderJob::~RenderJob() {
	this->view = NULL;
}

void * RenderJob::getSource() {
	return this->view;
}

void RenderJob::repaintRectangle(PageView * view, Rectangle * rect) {
	double zoom = view->xournal->getZoom();
	Document * doc = view->xournal->getDocument();

	doc->lock();

	double pageWidth = view->page->getWidth();
	double pageHeight = view->page->getHeight();

	doc->unlock();

	int x = rect->x * zoom;
	int y = rect->y * zoom;
	int width = rect->width * zoom;
	int height = rect->height * zoom;

	cairo_surface_t * rectBuffer = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
	cairo_t * crRect = cairo_create(rectBuffer);
	cairo_translate(crRect, -x, -y);
	cairo_scale(crRect, zoom, zoom);

	DocumentView v;
	v.limitArea(rect->x, rect->y, rect->width, rect->height);


	if (view->page->getBackgroundType() == BACKGROUND_TYPE_PDF) {
		int pgNo = view->page->getPdfPageNr();
		XojPopplerPage * popplerPage = doc->getPdfPage(pgNo);
		PdfCache * cache = view->xournal->getCache();
		PdfView::drawPage(cache, popplerPage, crRect, zoom, pageWidth, pageHeight);
	}

	doc->lock();
	v.drawPage(view->page, crRect, false);
	doc->unlock();

	cairo_destroy(crRect);

	g_mutex_lock(view->drawingMutex);
	cairo_t * crPageBuffer = cairo_create(view->crBuffer);

	cairo_set_operator(crPageBuffer, CAIRO_OPERATOR_SOURCE);
	cairo_set_source_surface(crPageBuffer, rectBuffer, x, y);
	cairo_rectangle(crPageBuffer, x, y, width, height);
	cairo_fill(crPageBuffer);

	cairo_destroy(crPageBuffer);

	cairo_surface_destroy(rectBuffer);

	g_mutex_unlock(view->drawingMutex);
}

void RenderJob::repaintRectangle(Rectangle * rect) {
	repaintRectangle(this->view, rect);
}

void RenderJob::run() {
	// TODO: !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//	CHECK_MEMORY(this);
//
//	double zoom = this->view->xournal->getZoom();
//
//	if (this->view->repaintComplete) {
//		Document * doc = this->view->xournal->getDocument();
//		GtkAllocation alloc;
//		gtk_widget_get_allocation(this->view->widget, &alloc);
//
//		cairo_surface_t * crBuffer = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, alloc.width, alloc.height);
//		cairo_t * cr2 = cairo_create(crBuffer);
//		cairo_scale(cr2, zoom, zoom);
//
//		XojPopplerPage * popplerPage = NULL;
//
//		doc->lock();
//
//		if (this->view->page->getBackgroundType() == BACKGROUND_TYPE_PDF) {
//			int pgNo = this->view->page->getPdfPageNr();
//			popplerPage = doc->getPdfPage(pgNo);
//		}
//
//		DocumentView view;
//		PdfView::drawPage(this->view->xournal->getCache(), popplerPage, cr2, zoom, this->view->page->getWidth(), this->view->page->getHeight());
//		view.drawPage(this->view->page, cr2, false);
//
//		cairo_destroy(cr2);
//		g_mutex_lock(this->view->drawingMutex);
//
//		if (this->view->crBuffer) {
//			cairo_surface_destroy(this->view->crBuffer);
//		}
//		this->view->crBuffer = crBuffer;
//
//		g_mutex_unlock(this->view->drawingMutex);
//		doc->unlock();
//
//		this->repaintComplete = true;
//	} else {
//		for (GList * l = this->view->repaintRect; l != NULL; l = l->next) {
//			Rectangle * rect = (Rectangle *) l->data;
//			repaintRectangle(rect);
//
//			this->repaintRect = g_list_append(repaintRect, new Rectangle(rect->x * zoom, rect->y * zoom, rect->width * zoom, rect->height * zoom));
//		}
//	}
//
//	CHECK_MEMORY(this->view);
//	g_mutex_lock(this->view->repaintRectMutex);
//
//	// delete all rectangles
//	this->view->repaintComplete = false;
//	for (GList * l = this->view->repaintRect; l != NULL; l = l->next) {
//		Rectangle * rect = (Rectangle *) l->data;
//		delete rect;
//	}
//	g_list_free(this->view->repaintRect);
//	this->view->repaintRect = NULL;
//
//	g_mutex_unlock(this->view->repaintRectMutex);

	callAfterRun();
}

void RenderJob::afterRun() {
	// TODO: !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//	if (this->repaintComplete) {
//		gtk_widget_queue_draw(this->view->widget);
//	} else {
//		for (GList * l = this->repaintRect; l != NULL; l = l->next) {
//			Rectangle * rect = (Rectangle *) l->data;
//			gtk_widget_queue_draw_area(this->view->widget, rect->x, rect->y, rect->width, rect->height);
//			delete rect;
//		}
//		g_list_free(this->repaintRect);
//		this->repaintRect = NULL;
//	}
}

JobType RenderJob::getType() {
	return JOB_TYPE_RENDER;
}