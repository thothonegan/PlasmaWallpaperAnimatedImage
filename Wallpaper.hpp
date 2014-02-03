#ifndef PLASMAWALLPAPERANIMATEDIMAGE_WALLPAPER_HPP
#define PLASMAWALLPAPERANIMATEDIMAGE_WALLPAPER_HPP

#include <Plasma/Wallpaper>

#include <QLabel>
#include <QMovie>
#include <QFlags>
#include <QString>

#include "ui_config.h"

const bool DebugEnabled = false;

class Wallpaper : public Plasma::Wallpaper
{
	Q_OBJECT
	public:
		Wallpaper (QObject* parent, const QVariantList& args);

		virtual void save (KConfigGroup& config);

		void paint (QPainter* painter, const QRectF& exposedRect);

		virtual QWidget* createConfigurationInterface (QWidget* parent);

		enum RenderOption {
			Default = 0x0,
			Stretch = 0x1,
			Aspect = 0x2,
			Tile = 0x4
		};
		Q_DECLARE_FLAGS( RenderOptions, RenderOption );

	Q_SIGNALS:
		void settingsChanged (bool modified);

	protected:
		virtual void init (const KConfigGroup& config);

	protected slots:
		void settingsModified();
		void selectFile();
		void setRenderOptions();
		void setAspectRatio();
		void setTiling();
		void frameChanged();
		void movieFinished();

	private:
		Ui::Config m_configWidget;

		QString m_filePath;
		RenderOptions m_renderOptions;
		QPoint m_aspectRatio;
		QPoint m_tiling;

		QMovie m_movie;
		QLabel m_displayedLabel;

		QPixmap m_buffer;

		void render (void);
		void alignDisplayedLabel (void);
};

Q_DECLARE_OPERATORS_FOR_FLAGS( Wallpaper::RenderOptions );

K_EXPORT_PLASMA_WALLPAPER (animatedimage, Wallpaper);

#endif // PLASMAWALLPAPERANIMATEDIMAGE_WALLPAPER_HPP

