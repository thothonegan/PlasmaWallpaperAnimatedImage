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
			Centered = 0x0,
			Scaled = 0x1,
			ScaledPreserveAspect = 0x2,
			Tiled = 0x4,
			TiledScaled = 0x8
		};
		Q_DECLARE_FLAGS( RenderOptions, RenderOption );

	Q_SIGNALS:
		void settingsChanged (bool modified);

	protected:
		virtual void init (const KConfigGroup& config);

	protected slots:
		void settingsModified();
		void selectFile();
		void setRenderOption();
		void setTiling();
		void frameChanged();
		void movieFinished();

	private:
		Ui::Config m_configWidget;

		QString m_filePath;
		RenderOptions m_renderOption;
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

