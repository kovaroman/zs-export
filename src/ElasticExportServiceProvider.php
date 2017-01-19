<?php

namespace ElasticExport;

use Plenty\Modules\DataExchange\Services\ExportPresetContainer;
use Plenty\Plugin\DataExchangeServiceProvider;

class ElasticExportServiceProvider extends DataExchangeServiceProvider
{
    const RAKUTEN_DE = 'RakutenDE';

	public function register()
	{
	}

	public function exports(ExportPresetContainer $container)
	{
		$formats = [
			'CdiscountCOM',
			'Check24DE',
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
            'TracdelightCOM',            
		];

		foreach ($formats as $format)
		{
            //todo richtige weiche einbauen
            if($format == self::RAKUTEN_DE)
            {
                $container->add(
                    $format,
                    'ElasticExport\ES_ResultFields\\'.$format,
                    'ElasticExport\ES_Generators\\'.$format,
                    'ElasticExport\Filters\\' . $format
                );
            }
            else
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
}
