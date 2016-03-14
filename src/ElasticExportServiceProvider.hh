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
			'BeezUp',
			'BelboonDE',
			'BilligerDE',
			'GeizhalsDE',
			'GoogleShopping',
			'GuenstigerDE',
			'Idealo',
			'KaufluxDE',
			'PreisRoboterDE',
			'RakutenDE',
			'ShopzillaDE',
			'ShoppingCOM',
            'BasicPriceSearchEngine',
            'KelkooBasicDE',
            'KelkooPremiumDE',
            'KuponaDE',
            'FashionDE',
            'EcondaDE',
            'MyBestBrandsDE',
            'TwengaCOM',
            'ZanoxDE',
            'SchuheDE',
			'ShippingProfiles',
            'Shopping24DE',
            'TracdelightCOM'
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
