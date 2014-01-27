#ifndef PLASMAWALLPAPERANIMATEDIMAGE_WALLPAPER_HPP
#define PLASMAWALLPAPERANIMATEDIMAGE_WALLPAPER_HPP

#include <Plasma/Wallpaper>

#include <QLabel>
#include <QMovie>

#include "ui_config.h"

class Wallpaper : public Plasma::Wallpaper
{
	Q_OBJECT
	public:
		Wallpaper (QObject* parent, const QVariantList& args);

		virtual void save (KConfigGroup& config);

		void paint (QPainter* painter, const QRectF& exposedRect);

		virtual QWidget* createConfigurationInterface (QWidget* parent);

	Q_SIGNALS:
		void settingsChanged (bool modified);

	protected:
		virtual void init (const KConfigGroup& config);

	protected slots:
		void settingsModified();
		void selectFile();
		void frameChanged();
		void movieFinished();

	private:
		Ui::Config m_configWidget;

		QString m_filePath;

		QMovie m_movie;
		QLabel m_displayedLabel;

		QPixmap m_buffer;

		void render (void);
};

K_EXPORT_PLASMA_WALLPAPER (animatedimage, Wallpaper);

#endif // PLASMAWALLPAPERANIMATEDIMAGE_WALLPAPER_HPP

