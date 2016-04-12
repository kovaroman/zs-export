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
			'CdiscountCOM',
			'BeezUp',
			'BelboonDE',
			'BilligerDE',
			'GeizhalsDE',
			'GoogleShopping',
			'GuenstigerDE',
			'IdealoDE',
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
            'TreepodiaCOM',
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
				$format,
				'ElasticExport\ResultFields\\'.$format,
				'ElasticExport\Generators\\'.$format,
				'ElasticExport\Filters\\' . $format
			);
		}
	}
}
