
#include <cmath>

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

	connect (&m_movie, SIGNAL(frameChanged(int)), this, SLOT(frameChanged()));
}

void Wallpaper::save (KConfigGroup& config)
{
	if ( DebugEnabled ) qDebug() << "save";

	config.writeEntry ("filePath", m_filePath);
	config.writeEntry( "renderOption", (int)m_renderOption );
	config.writeEntry( "tiling", m_tiling );
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

	int index = (int)m_renderOption - 1;
	if ( index > 2 ) index = log( m_renderOption ) / log(2);
	m_configWidget.renderOption->setCurrentIndex( index );
	connect( m_configWidget.renderOption, SIGNAL( currentIndexChanged(int) ), this, SLOT( setRenderOption() ) );

	m_configWidget.tilingX->setText( QString().setNum( m_tiling.x() ) );
	m_configWidget.tilingY->setText( QString().setNum( m_tiling.y() ) );
	connect( m_configWidget.tilingX, SIGNAL( textChanged(const QString &) ), this, SLOT( setTiling() ) );
	connect( m_configWidget.tilingY, SIGNAL( textChanged(const QString &) ), this, SLOT( setTiling() ) );

	connect (this, SIGNAL(settingsChanged(bool)), parent, SLOT(settingsChanged(bool)));

	return widget;
}

void Wallpaper::init (const KConfigGroup& config)
{
	m_filePath = config.readEntry ("filePath", QString());
	m_renderOption = (Wallpaper::RenderOptions)config.readEntry( "renderOption", 0 );
	m_tiling = config.readEntry( "tiling", QPoint() );

	if ( DebugEnabled ) qDebug() << "init: " << m_filePath;

	#if FORCE_IMAGE
		m_filePath = FORCE_IMAGE_TO;
	#endif

	m_movie.stop();
	m_movie.setFileName(m_filePath);
	m_movie.start();

	emit update(boundingRect());
}

void Wallpaper::settingsModified (void)
{
	if ( DebugEnabled )
	{
		qDebug() << "SETTINGS CHANGED";
		qDebug() << "Current File Path : " << m_filePath;
	}
	m_configWidget.m_filePath->setText (m_filePath);

	m_movie.stop();
	m_movie.setFileName (m_filePath);
	m_movie.start();

	emit settingsChanged(true);
	emit update(boundingRect());
}

void Wallpaper::selectFile (void)
{
	if ( DebugEnabled ) qDebug() << "SELECT FILE";

	QString filePath = QFileDialog::getOpenFileName (NULL, "Select Animated Image", QString(), "Images (*.gif)");
	if (filePath != QString())
	{
		m_filePath = filePath;
		settingsModified();
	}
}

void Wallpaper::setRenderOption( void )
{
	if ( DebugEnabled ) qDebug() << "SET RENDER OPTIONS";

	switch ( m_configWidget.renderOption->currentIndex() )
	{
		case 1:
			m_renderOption = Wallpaper::Scaled;
			break;
		case 2:
			m_renderOption = Wallpaper::ScaledPreserveAspect;
			break;
		case 3:
			m_renderOption = Wallpaper::Tiled;
			break;
		case 4:
			m_renderOption = Wallpaper::TiledScaled;
			break;
		case 0:
		default:
			m_renderOption = Wallpaper::Centered;
			break;
	}

	settingsModified();
}

void Wallpaper::setTiling( void )
{
	if ( DebugEnabled ) qDebug() << "SET TILING";

	bool ok;
	int tilingX = m_configWidget.tilingX->text().toInt( &ok );
	if ( ( tilingX < 1 ) || ( !ok ) )
	{
		tilingX = 1;
		m_configWidget.tilingX->setText( QString().setNum( tilingX ) );
	}
	m_tiling.setX( tilingX );

	int tilingY = m_configWidget.tilingY->text().toInt( &ok );
	if ( ( tilingY < 1 ) || ( !ok ) )
	{
		tilingY = 1;
		m_configWidget.tilingY->setText( QString().setNum( tilingY ) );
	}
	m_tiling.setY( tilingY );

	settingsModified();
}

void Wallpaper::frameChanged (void)
{
	if ( DebugEnabled ) qDebug() << "***** frame change ****";

	// repaint
	render();
	emit update(boundingRect());
}

void Wallpaper::render (void)
{
	QSize renderSize = boundingRect().size().toSize();
	QSize imageSize = m_movie.frameRect().size();
	if ( m_buffer.size() != boundingRect().size() ) m_buffer = QPixmap( renderSize );

	// This code was intended to be in settingsModified(), but boundingRect() wasn't returning the
	// expected values.  If you figure out how to fix this, put this code back into settingsModified().
	if ( m_renderOption.testFlag( Wallpaper::TiledScaled ) )
	{
		// Scale so that the number of tiles fit into the screen space
		QSize scaledTiling = renderSize;
		scaledTiling.rwidth() /= m_tiling.x();
		scaledTiling.rheight() /= m_tiling.y();
		m_movie.setScaledSize( scaledTiling );
		m_displayedLabel.resize( scaledTiling );
	}
	else if ( m_renderOption.testFlag( Wallpaper::Scaled ) )
	{
		// Scale to size of the screen
		m_movie.setScaledSize( renderSize );
		m_displayedLabel.resize( renderSize );
	}
	else if ( m_renderOption.testFlag( Wallpaper::ScaledPreserveAspect ) )
	{
		// Scale to size of the screen, preserving the aspect ratio of the original image
		QSize aspectScale;
		if ( imageSize.width() > imageSize.height() )
		{
			aspectScale = QSize( renderSize.width(), imageSize.height() * renderSize.width() / imageSize.width() );
		} else {
			aspectScale = QSize( imageSize.width() * renderSize.height() / imageSize.height(), renderSize.height() );
		}

		m_movie.setScaledSize( aspectScale );
		m_displayedLabel.resize( aspectScale );
	}
	else
	{
		// Reset to original size
		m_movie.setScaledSize( QSize( -1, -1 ) );
		m_displayedLabel.resize( imageSize );
	}

	QPainter painter;
	painter.begin (&m_buffer);

	if ( ( m_renderOption.testFlag( Wallpaper::Tiled ) ) || ( m_renderOption.testFlag( Wallpaper::TiledScaled ) ) )
	{
		for ( int x = 0; x < m_tiling.x(); x++ ) {
			for ( int y = 0; y < m_tiling.y(); y++ ) {
				QPoint offset = QPoint( imageSize.width() * x + 1, imageSize.height() * y + 1 );
				m_displayedLabel.render( &painter, offset );
			}
		}
	}
	else if ( m_renderOption.testFlag( Wallpaper::Centered ) )
	{
		QPoint offset = QPoint( renderSize.width() / 2 - imageSize.width() / 2, renderSize.height() / 2 - imageSize.height() / 2 );
		m_displayedLabel.render( &painter, offset );
	}
	else
	{
		m_displayedLabel.render (&painter);
	}
}

#include "Wallpaper.moc"

