<?hh // strict
namespace ElasticExport;

use Plenty\Modules\DataExchange\Services\ExportPresetContainer;
use Plenty\Plugin\DataExchangeServiceProvider;

class ElasticExportServiceProvider extends DataExchangeServiceProvider
{
	public function register():void
	{

	}

	public function exports(ExportPresetContainer $container):void
	{
		$formats = [
			'Belboon',
			'Billiger',
			'Geizhals',
			'GoogleShopping',
			'Guenstiger',
			'Idealo',
			'PreisRoboter',
			'Rakuten',
			'Shopzilla',
			'Shopping',
		];

		foreach ($formats as $format)
		{
			$container->add(
				$format.'Format',
				'ElasticExport\ResultFields\\'.$format,
				'ElasticExport\Generators\\'.$format,
				'ElasticExport\Filters\\' . $format
			);
		}
	}
}
