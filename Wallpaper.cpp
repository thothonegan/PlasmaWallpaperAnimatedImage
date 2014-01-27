#include <QFileDialog>
#include <QPainter>

#include "Wallpaper.hpp"

// useful for debugging - set to 0 to disable
#define FORCE_IMAGE 0
#define FORCE_IMAGE_TO "/home/thothonegan/1390774577226.gif"

Wallpaper::Wallpaper (QObject* parent, const QVariantList& args)
: Plasma::Wallpaper (parent, args)
{
	m_movie.setCacheMode (QMovie::CacheAll);
	m_displayedLabel.setMovie (&m_movie);
	//m_displayedLabel.setScaledContents(true);

	connect (&m_movie, SIGNAL(finished()), this, SLOT(movieFinished()));
	connect (&m_movie, SIGNAL(frameChanged(int)), this, SLOT(frameChanged()));

	
	//m_filePath = "/home/thothonegan/1390774577226.gif";
	//settingsModified();
	//emit settingsChanged(true);
}

void Wallpaper::save (KConfigGroup& config)
{
	//qDebug() << "save";

	config.writeEntry ("filePath", m_filePath);
}

void Wallpaper::paint (QPainter* painter, const QRectF& exposedRect)
{
	if (m_buffer.size() != boundingRect().size())
	{
		render();
	}

	painter->drawPixmap (exposedRect, m_buffer, exposedRect);
}

QWidget* Wallpaper::createConfigurationInterface (QWidget* parent)
{
	QWidget* widget = new QWidget (parent);
	m_configWidget.setupUi (widget);

	m_configWidget.m_filePath->setText (m_filePath);
	connect (m_configWidget.m_selectFileButton, SIGNAL(clicked()), this, SLOT(selectFile()));

	connect (this, SIGNAL(settingsChanged(bool)), parent, SLOT(settingsChanged(bool)));

	return widget;
}

void Wallpaper::init (const KConfigGroup& config)
{
	m_filePath = config.readEntry ("filePath", QString());

	//qDebug() << "init: " << m_filePath;

	#if FORCE_IMAGE
		m_filePath = FORCE_IMAGE_TO;
	#endif

	m_movie.stop();
	m_movie.setFileName(m_filePath);
	m_movie.start();

	render();
	emit update(boundingRect());
}

void Wallpaper::settingsModified (void)
{
	//qDebug() << "settingsModified : " << m_filePath;
	m_configWidget.m_filePath->setText (m_filePath);

	m_movie.stop();
	m_movie.setFileName (m_filePath);
	m_movie.start();

	emit settingsChanged(true);

	render();
	emit update(boundingRect());
}

void Wallpaper::selectFile (void)
{
	//qDebug() << "SELECT FILE";

	QString filePath = QFileDialog::getOpenFileName (NULL, "Select Animated Image", QString(), "Images (*.gif)");
	if (filePath != QString())
	{
		m_filePath = filePath;

	//	qDebug() << "SETTINGS CHANGED";
		settingsModified();
	}
}

void Wallpaper::frameChanged (void)
{
//qDebug() << "***** frame change ****";

	// repaint
	render();
	emit update(boundingRect());
}

void Wallpaper::movieFinished (void)
{
	//qDebug() << "*** movie finished";
//	m_movie.start(); // repeat
}

void Wallpaper::render (void)
{
	if (m_buffer.size() != boundingRect().size())
	{
		m_buffer = QPixmap(boundingRect().size().toSize());
		m_movie.setScaledSize (boundingRect().size().toSize());
		m_displayedLabel.resize (boundingRect().size().toSize());
	}

	QPainter painter;
	painter.begin (&m_buffer);

	m_displayedLabel.render (&painter);
}

#include "Wallpaper.moc"

