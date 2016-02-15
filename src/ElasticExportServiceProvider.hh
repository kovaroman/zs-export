<?hh // strict
namespace ElasticExport;

use ElasticExport\Generators\BilligerGenerator;
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
			'Billiger',
			'Geizhals',
            'Guenstiger',
			'Idealo',
			'PreisRoboter',
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
